#!/bin/bash
# cleanup_for_github.sh
# Run this script before uploading to GitHub

echo "🧹 Cleaning up project for GitHub upload..."

# Remove compiled object files
echo "Removing compiled files..."
rm -f *.o
rm -f game

# Remove SFML library (too large for GitHub)
echo "Removing SFML library (users will install it themselves)..."
rm -rf SFML-2.5.1
rm -f SFML-*.zip
rm -f *.zip

# Remove CMake build files
echo "Removing CMake build artifacts..."
rm -rf CMakeFiles/
rm -f CMakeCache.txt
rm -f cmake_install.cmake
rm -f Makefile

# Optional: Clear personal test data (uncomment if needed)
# echo "Clearing personal test data..."
# echo "" > users.txt
# echo "" > friends.txt
# echo "" > leaderboard.txt
# find saves/ -name "*.sav" -type f -delete

echo ""
echo "✅ Cleanup complete!"
echo ""
echo "Current directory size:"
du -sh .
echo ""
echo "Next steps:"
echo "1. Review the changes: ls -lh"
echo "2. Check .gitignore: cat .gitignore"
echo "3. Initialize git: git init"
echo "4. Add files: git add ."
echo "5. Commit: git commit -m 'Initial commit'"
echo ""
echo "See GITHUB_UPLOAD_GUIDE.md for detailed instructions!"
