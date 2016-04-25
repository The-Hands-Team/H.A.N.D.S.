rm -r tests/
mkdir tests
mkdir tests/TRASH
mkdir tests/test_dir
touch tests/test_dir/file1.txt
printf "content" >> tests/test_dir/file1.txt
touch tests/test_dir/file2.txt
printf "content" >> tests/test_dir/file2.txt
mkdir tests/test_dir/dir1
mkdir tests/test_dir/dir2
touch tests/test_dir/dir1/sdfile1_1.txt
printf "content" >> tests/test_dir/dir1/sdfile1_1.txt
touch tests/test_dir/dir1/sdfile1_2.txt
printf "content" >> tests/test_dir/dir1/file1_2.txt
mkdir tests/test_dir/dir1/sdir1
touch tests/test_dir/dir1/sdir1/ssdfile1_1_1.txt
printf "content" >> tests/test_dir/dir1/sdir1/ssdfile1_1_1.txt
touch tests/test_dir/dir2/sdfile2_1.txt
printf "content" >> tests/test_dir/dir2/sdfile2_1.txt
