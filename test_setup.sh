if [ ! -d tests ]
    then
        mkdir tests
fi
rm -r tests/TRASH
rm -r tests/test_dir
rm -r tests/CLIPBOARD
mkdir tests/TRASH
mkdir tests/test_dir
mkdir tests/CLIPBOARD
if [ -a tests/example.pdf ]
    then
        cp tests/example.pdf tests/test_dir/example.pdf
fi
touch tests/test_dir/file1.txt
printf "content" >> tests/test_dir/file1.txt
touch tests/test_dir/file2.txt
printf "content" >> tests/test_dir/file2.txt

fnum=30
while [ $fnum -gt 0 ]
do
touch tests/test_dir/f$fnum
printf "content" >> tests/test_dir/f$fnum
fnum=`expr $fnum - 1`
done

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
