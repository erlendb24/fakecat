# fakecat

fakecat, tries to do most things FreeBSD cat does within the limitations of KNR 8.1. 
The exercise equires the use of `open()`, `close()`, `read()` and `write()`, which ends up making 
outputting stdin very impractical.

flags:  
-b number non blank lines  
-n number all lines  
-e $ at the end of lines  
-s squeeze lines (no more than one newline at a time)  
-t Tab characters are printed as ^I

-b overrides -n when used in conjunction
