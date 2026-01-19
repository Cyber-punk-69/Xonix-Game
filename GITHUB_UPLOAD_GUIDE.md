# GitHub Upload Guide for Xonix Project

## Step-by-Step Instructions

### Step 1: Clean Up Before Upload

**Remove large/unnecessary files:**

```bash
cd "/home/punk/Downloads/111/xonic project4"

# Remove compiled files
rm -f *.o game

# Remove SFML library (users will install it themselves)
rm -rf SFML-2.5.1
rm -f SFML-2.5.1-sources.zip

# Optional: Clear personal test data
# Uncomment these if you want to start fresh
# echo "" > users.txt
# echo "" > friends.txt
# echo "" > leaderboard.txt
# rm -rf saves/*.sav
```

### Step 2: Initialize Git Repository

```bash
# Navigate to project directory
cd "/home/punk/Downloads/111/xonic project4"

# Initialize git
git init

# Add all files (respecting .gitignore)
git add .

# Check what will be committed
git status

# Make first commit
git commit -m "Initial commit: Xonix tile-capture game with multiplayer and social features"
```

### Step 3: Create GitHub Repository

1. **Go to GitHub**: https://github.com
2. **Click**: "+" icon (top right) → "New repository"
3. **Fill in**:
   - Repository name: `xonix-game` (or your choice)
   - Description: "Classic Xonix arcade game built with C++ and SFML"
   - Public or Private: Choose based on preference
   - **DO NOT** check "Initialize with README" (we already have one)
4. **Click**: "Create repository"

### Step 4: Connect Local Repo to GitHub

GitHub will show you commands. Use these:

```bash
# Add remote origin (replace YOUR_USERNAME with your GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/xonix-game.git

# Rename branch to main (if needed)
git branch -M main

# Push to GitHub
git push -u origin main
```

### Step 5: Update README with Your Info

Before pushing, update these in README.md:

```bash
# Open README.md and replace:
# - YOUR_USERNAME → your GitHub username
# - [Your Name] → your actual name
# - [Name 2] → your partner's name (if applicable)
```

Then commit the changes:

```bash
git add README.md
git commit -m "Update README with author information"
git push
```

### Step 6: Add Topics/Tags on GitHub

On your repository page:
1. Click "⚙️ Settings" or find "About" section
2. Add topics: `cpp`, `sfml`, `game`, `xonix`, `arcade`, `data-structures`, `dsa-project`
3. Add description and website if you have one

### Step 7: Create a Release (Optional but Recommended)

```bash
# Create a tag for version 1.0
git tag -a v1.0 -m "Version 1.0 - Initial release"

# Push the tag
git push origin v1.0
```

Then on GitHub:
1. Go to "Releases" tab
2. Click "Create a new release"
3. Choose tag `v1.0`
4. Title: "Xonix Game v1.0"
5. Description: Add features list
6. Click "Publish release"

## Alternative: Using GitHub CLI

If you have GitHub CLI installed:

```bash
# Login to GitHub
gh auth login

# Create repo and push
gh repo create xonix-game --public --source=. --remote=origin --push

# Done!
```

## Quick Reference Commands

```bash
# Check status
git status

# Add files
git add .

# Commit changes
git commit -m "Your message here"

# Push changes
git push

# Pull latest changes
git pull

# View commit history
git log --oneline

# Create new branch
git checkout -b feature-name

# Switch branches
git checkout main
```

## Troubleshooting

### Error: "Repository not found"
- Check the remote URL: `git remote -v`
- Make sure repository exists on GitHub
- Verify you have access permissions

### Error: "Permission denied (publickey)"
- Set up SSH keys or use HTTPS
- For HTTPS: Use personal access token instead of password

### Large Files Warning
- SFML library should be removed (covered in .gitignore)
- If you get warnings, make sure .gitignore is working:
  ```bash
  git rm -r --cached SFML-2.5.1
  git commit -m "Remove SFML from repository"
  ```

## What Gets Uploaded

✅ **Included:**
- Source code (.cpp, .h files)
- Documentation (README.md, CONTRIBUTING.md, etc.)
- Build scripts (p.sh, CMakeLists.txt)
- Assets (fonts/, images/)
- Configuration files (*.txt, *.cfg)
- Empty saves/ directory structure

❌ **Excluded (via .gitignore):**
- Compiled files (*.o, game executable)
- SFML library
- IDE files
- Build artifacts

## Best Practices

1. **Write descriptive commit messages**
2. **Commit frequently** (small, logical changes)
3. **Always test before pushing**
4. **Don't commit sensitive data** (passwords, API keys)
5. **Update README** when adding features
6. **Use branches** for new features
7. **Review changes** before committing: `git diff`

## After Upload

1. **Add a screenshot** to the repository
2. **Enable GitHub Pages** if you want a project website
3. **Star your own repo** (why not! 😄)
4. **Share the link** with friends/on LinkedIn
5. **Keep updating** as you improve the game

---

Your project link will be:
**https://github.com/YOUR_USERNAME/xonix-game**

Good luck! 🚀
