@echo off
git pull
echo n | compile
git add .
git commit -m "upload files (auto)"
git push -u origin main
