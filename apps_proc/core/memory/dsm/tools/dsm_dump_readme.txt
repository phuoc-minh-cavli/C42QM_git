T32>do dsm3.cmm c:\temp

This will create a dump file at c:\temp\dsm_memory_pool_dump.txt

2) Run the perl script,dsm.pl

 C:\temp>perl dsm.pl dsm_memory_pool_dump.txt   > myDump.csv

3) Excel Spreadsheets will work fine for displaying:
C:\temp>myDump.csv

 

To check the currently used dsm items from a particular pool, use the following procedure :

1. Open the file in Excel and select  Menu "Data ->Filter".

2. Select a particular pool in "# Pool" Column.( Typical values in  Size column can help to identify Large Item/Small Item/ DUP Item pool).

     Large Item pool size - 1536

     Small Item pool size - 64/128

     Dup Item Pool Size = 0
