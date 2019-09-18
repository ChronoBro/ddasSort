parallel --xapply ./sort4 ::: 80 101 121 141 161 181 201 221 241 261 ::: 100 120 140 160 180 200 220 240 260 285 ::: sort4Set1 sort4Set2 sort4Set3 sort4Set4 sort4Set5 sort4Set6 sort4Set7 sort4Set8 sort4Set9 sort4Set10
cd root-files/
hadd -f "$1" sort4Set1.root sort4Set2.root sort4Set3.root sort4Set4.root sort4Set5.root sort4Set6.root sort4Set7.root sort4Set8.root sort4Set9.root sort4Set10.root

