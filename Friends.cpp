#include "Friends.h"
#include <cstring>
#include <cstdlib>
#include <ctype.h>

static const int GROW_STEP = 64;

FriendSystem::FriendSystem(int initialCapacity, int hashSize)
{
    capacity = initialCapacity;
    players = (PlayerEntry *)malloc(sizeof(PlayerEntry) * capacity);
    for (int i = 0; i < capacity; ++i)
        new (&players[i]) PlayerEntry();
    count = 0;

    tableSize = hashSize;
    hashKeys = (char **)malloc(sizeof(char *) * tableSize);
    hashVals = (int *)malloc(sizeof(int) * tableSize);
    for (int i = 0; i < tableSize; ++i)
    {
        hashKeys[i] = nullptr;
        hashVals[i] = -1;
    }
}

FriendSystem::~FriendSystem()
{
    freeListNodes();
    if (players)
        free(players);
    if (hashKeys)
    {
        for (int i = 0; i < tableSize; ++i)
            if (hashKeys[i])
                free(hashKeys[i]);
        free(hashKeys);
    }
    if (hashVals)
        free(hashVals);
}

int FriendSystem::hash(const char *s) const
{
    // simple djb2-like
    unsigned long h = 5381;
    for (const char *p = s; *p; ++p)
        h = ((h << 5) + h) + (unsigned char)(*p);
    return (int)(h % (unsigned long)tableSize);
}

void FriendSystem::grow()
{
    int newCap = capacity + GROW_STEP;
    PlayerEntry *np = (PlayerEntry *)malloc(sizeof(PlayerEntry) * newCap);
    for (int i = 0; i < newCap; ++i)
        new (&np[i]) PlayerEntry();
    for (int i = 0; i < count; ++i)
        np[i] = players[i];
    free(players);
    players = np;
    capacity = newCap;
}

int FriendSystem::ensurePlayer(const std::string &username)
{
    if (username.empty())
        return -1;
    int idx = findIndex(username);
    if (idx >= 0)
        return idx;

    if (count >= capacity)
        grow();
    // create new player
    strncpy(players[count].username, username.c_str(), 63);
    players[count].username[63] = '\0';
    players[count].friends = nullptr;
    players[count].pending = nullptr;

    // insert into hash table (linear probing)
    int h = hash(players[count].username);
    int start = h;
    while (hashKeys[h] != nullptr)
    {
        h = (h + 1) % tableSize;
        if (h == start)
            break;
    }

    // store key string copy
    hashKeys[h] = (char *)malloc(strlen(players[count].username) + 1);
    strcpy(hashKeys[h], players[count].username);
    hashVals[h] = count;

    ++count;
    return count - 1;
}

int FriendSystem::findIndex(const std::string &username) const
{
    if (username.empty())
        return -1;
    int h = hash(username.c_str());
    int start = h;
    while (hashKeys[h] != nullptr)
    {
        if (strcmp(hashKeys[h], username.c_str()) == 0)
            return hashVals[h];
        h = (h + 1) % tableSize;
        if (h == start)
            break;
    }
    return -1;
}

static void appendFriendNode(FriendNode *&head, const char *name)
{
    FriendNode *n = (FriendNode *)malloc(sizeof(FriendNode));
    // placement new
    new (n) FriendNode(name);
    n->next = head;
    head = n;
}

static void appendPendingNode(PendingNode *&head, const char *name)
{
    PendingNode *n = (PendingNode *)malloc(sizeof(PendingNode));
    new (n) PendingNode(name);
    n->next = head;
    head = n;
}

bool FriendSystem::isAlreadyFriends(int aIdx, const char *bname) const
{
    FriendNode *f = players[aIdx].friends;
    while (f)
    {
        if (strcmp(f->name, bname) == 0)
            return true;
        f = f->next;
    }
    return false;
}

bool FriendSystem::pendingExists(int idx, const char *from) const
{
    PendingNode *p = players[idx].pending;
    while (p)
    {
        if (strcmp(p->name, from) == 0)
            return true;
        p = p->next;
    }
    return false;
}

void FriendSystem::addFriendByIndex(int aIdx, int bIdx)
{
    // add b to a's list and a to b's list if not already
    if (!isAlreadyFriends(aIdx, players[bIdx].username))
        appendFriendNode(players[aIdx].friends, players[bIdx].username);
    if (!isAlreadyFriends(bIdx, players[aIdx].username))
        appendFriendNode(players[bIdx].friends, players[aIdx].username);
}

bool FriendSystem::sendFriendRequest(const std::string &from, const std::string &to)
{
    if (from.empty() || to.empty())
        return false;
    int a = findIndex(from);
    int b = findIndex(to);
    if (a < 0 || b < 0)
        return false;
    if (a == b)
        return false;
    if (isAlreadyFriends(a, players[b].username))
        return false;
    if (pendingExists(b, players[a].username))
        return false;

    appendPendingNode(players[b].pending, players[a].username);
    // persist change
    saveToFile("friends.txt");
    return true;
}

bool FriendSystem::acceptFriendRequest(const std::string &to, const std::string &from)
{
    int tidx = findIndex(to);
    int fidx = findIndex(from);
    if (tidx < 0 || fidx < 0)
        return false;
    PendingNode *prev = nullptr;
    PendingNode *cur = players[tidx].pending;
    while (cur)
    {
        if (strcmp(cur->name, from.c_str()) == 0)
        {
            if (prev)
                prev->next = cur->next;
            else
                players[tidx].pending = cur->next;
            cur->~PendingNode();
            free(cur);
            addFriendByIndex(tidx, fidx);
            saveToFile("friends.txt");
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

bool FriendSystem::rejectFriendRequest(const std::string &to, const std::string &from)
{
    int tidx = findIndex(to);
    if (tidx < 0)
        return false;
    PendingNode *prev = nullptr;
    PendingNode *cur = players[tidx].pending;
    while (cur)
    {
        if (strcmp(cur->name, from.c_str()) == 0)
        {
            if (prev)
                prev->next = cur->next;
            else
                players[tidx].pending = cur->next;
            cur->~PendingNode();
            free(cur);
            saveToFile("friends.txt");
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

bool FriendSystem::saveToFile(const char *path) const
{
    FILE *f = fopen(path, "w");
    if (!f)
        return false;
    // format: username|friendA,friendB|pendingX,pendingY
    for (int i = 0; i < count; ++i)
    {
        fprintf(f, "%s|", players[i].username);
        // friends
        FriendNode *fn = players[i].friends;
        bool first = true;
        for (FriendNode *p = fn; p; p = p->next)
        {
            if (!first)
                fprintf(f, ",");
            fprintf(f, "%s", p->name);
            first = false;
        }
        fprintf(f, "|");
        // pending
        PendingNode *pn = players[i].pending;
        first = true;
        for (PendingNode *p = pn; p; p = p->next)
        {
            if (!first)
                fprintf(f, ",");
            fprintf(f, "%s", p->name);
            first = false;
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return true;
}

static void trimInPlace(char *s)
{
    if (!s)
        return;
    // trim leading
    char *p = s;
    while (*p && isspace((unsigned char)*p))
        ++p;
    if (p != s)
        memmove(s, p, strlen(p) + 1);
    // trim trailing
    int L = strlen(s);
    while (L > 0 && isspace((unsigned char)s[L - 1]))
        s[--L] = '\0';
}

static void stripCharEdges(char *s, char c)
{
    if (!s)
        return;
    // strip leading
    char *p = s;
    while (*p == c)
        ++p;
    if (p != s)
        memmove(s, p, strlen(p) + 1);
    // strip trailing
    int L = strlen(s);
    while (L > 0 && s[L - 1] == c)
        s[--L] = '\0';
}

static bool looksLikeLongNumber(const char *s)
{
    int len = 0;
    int digits = 0;
    for (const char *p = s; *p; ++p)
    {
        ++len;
        if (isdigit((unsigned char)*p))
            ++digits;
    }
    if (len >= 6 && digits == len)
        return true; // long all-digits token -> likely timestamp/id
    return false;
}

static bool looksLikeDate(const char *s)
{
    // simple YYYY-MM-DD check
    if (!s)
        return false;
    int L = strlen(s);
    if (L != 10)
        return false;
    for (int i = 0; i < 10; ++i)
    {
        if (i == 4 || i == 7)
        {
            if (s[i] != '-')
                return false;
        }
        else
        {
            if (!isdigit((unsigned char)s[i]))
                return false;
        }
    }
    return true;
}

bool FriendSystem::loadFromFile(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return false;
    char line[1024];
    while (fgets(line, sizeof(line), f))
    {
        // remove newline
        int L = strlen(line);
        while (L > 0 && (line[L - 1] == '\n' || line[L - 1] == '\r'))
            line[--L] = '\0';
        char *p = line;
        // split into three parts by '|'
        char *bar1 = strchr(p, '|');
        if (!bar1)
            continue;
        *bar1 = '\0';
        char *username = p;
        char *rest = bar1 + 1;
        char *bar2 = strchr(rest, '|');
        char *friendsStr = rest;
        char *pendingStr = nullptr;
        if (bar2)
        {
            *bar2 = '\0';
            pendingStr = bar2 + 1;
        }

        trimInPlace(username);
        trimInPlace(friendsStr);
        if (pendingStr)
            trimInPlace(pendingStr);

        // strip stray pipe characters at edges
        stripCharEdges(username, '|');
        stripCharEdges(friendsStr, '|');
        if (pendingStr)
            stripCharEdges(pendingStr, '|');

        if (username[0] == '\0')
            continue;
        int idx = ensurePlayer(std::string(username));
        if (idx < 0)
            continue;

        // parse friends CSV
        char *cur = friendsStr;
        while (cur && *cur)
        {
            // find comma
            char *comma = strchr(cur, ',');
            if (comma)
            {
                *comma = '\0';
            }
            trimInPlace(cur);
            // strip pipes from token edges
            stripCharEdges(cur, '|');
            if (cur[0] != '\0' && !looksLikeLongNumber(cur) && !looksLikeDate(cur))
            {
                int fi = findIndex(std::string(cur));
                if (fi < 0)
                    fi = ensurePlayer(std::string(cur));
                if (fi >= 0 && fi != idx)
                    addFriendByIndex(idx, fi);
            }
            if (!comma)
                break;
            cur = comma + 1;
        }

        // parse pending CSV
        if (pendingStr)
        {
            cur = pendingStr;
            while (cur && *cur)
            {
                char *comma = strchr(cur, ',');
                if (comma)
                {
                    *comma = '\0';
                }
                trimInPlace(cur);
                stripCharEdges(cur, '|');
                if (cur[0] != '\0' && !looksLikeLongNumber(cur) && !looksLikeDate(cur))
                {
                    // ensure target player exists
                    ensurePlayer(std::string(cur));
                    appendPendingNode(players[idx].pending, cur);
                }
                if (!comma)
                    break;
                cur = comma + 1;
            }
        }
    }
    fclose(f);
    return true;
}

void FriendSystem::ensurePlayersFromFile(const char *usersPath)
{
    FILE *f = fopen(usersPath, "r");
    if (!f)
        return;
    char buf[512];
    while (fgets(buf, sizeof(buf), f))
    {
        // trim newline and whitespace
        int L = strlen(buf);
        while (L > 0 && (buf[L - 1] == '\n' || buf[L - 1] == '\r'))
            buf[--L] = '\0';
        trimInPlace(buf);
        if (buf[0] == '\0')
            continue;
        // first token
        char uname[128];
        if (sscanf(buf, "%127s", uname) == 1)
        {
            stripCharEdges(uname, '|');
            // skip long numeric tokens or date-like tokens
            if (!looksLikeLongNumber(uname) && !looksLikeDate(uname))
                ensurePlayer(std::string(uname));
        }
    }
    fclose(f);
}

int FriendSystem::getFriendCount(const std::string &username) const
{
    int idx = findIndex(username);
    if (idx < 0)
        return 0;
    int c = 0;
    FriendNode *f = players[idx].friends;
    while (f)
    {
        ++c;
        f = f->next;
    }
    return c;
}

bool FriendSystem::getFriendAt(const std::string &username, int idxPos, std::string &outName) const
{
    int idx = findIndex(username);
    if (idx < 0)
        return false;
    int c = 0;
    FriendNode *f = players[idx].friends;
    while (f)
    {
        if (c == idxPos)
        {
            outName = std::string(f->name);
            return true;
        }
        ++c;
        f = f->next;
    }
    return false;
}

int FriendSystem::getPendingCount(const std::string &username) const
{
    int idx = findIndex(username);
    if (idx < 0)
        return 0;
    int c = 0;
    PendingNode *p = players[idx].pending;
    while (p)
    {
        ++c;
        p = p->next;
    }
    return c;
}

bool FriendSystem::getPendingAt(const std::string &username, int idxPos, std::string &outName) const
{
    int idx = findIndex(username);
    if (idx < 0)
        return false;
    int c = 0;
    PendingNode *p = players[idx].pending;
    while (p)
    {
        if (c == idxPos)
        {
            outName = std::string(p->name);
            return true;
        }
        ++c;
        p = p->next;
    }
    return false;
}

void FriendSystem::freeListNodes()
{
    if (!players)
        return;
    for (int i = 0; i < count; ++i)
    {
        FriendNode *f = players[i].friends;
        while (f)
        {
            FriendNode *n = f->next;
            f->~FriendNode();
            free(f);
            f = n;
        }
        players[i].friends = nullptr;
        PendingNode *p = players[i].pending;
        while (p)
        {
            PendingNode *n = p->next;
            p->~PendingNode();
            free(p);
            p = n;
        }
        players[i].pending = nullptr;
    }
}
