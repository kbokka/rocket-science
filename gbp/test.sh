#! /bin/sh

make
read

proj5 r6_sp.txt r6_out.pck | tee r6_out.log
diff r6_out.pck r6.pck
diff r6_out.log r6.log
read

FILES=sp_flr/*.txt
for f in $FILES
do
  echo 
  echo "Processing $f file..." 
  echo "proj5 $f out${f} | tee scrn${f}"
  proj5 $f out${f} | tee scrn${f}
done

FILES=sp_flr/*.txt
for f in $FILES
do
  echo >> report.txt
  echo "Processing $f file..." 
  echo "proj5 $f /dev/null >> report.txt" >> report.txt
  proj5 $f /dev/null >> report.txt
done
