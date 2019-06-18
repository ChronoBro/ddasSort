parallel --xapply ./sort4 ::: 80 121 161 201 241 ::: 120 160 200 240 285 ::: sort4Set1 sort4Set2 sort4Set3 sort4Set4 sort4Set5
cd root-files/
hadd -f "$1" sort4Set1.root sort4Set2.root sort4Set3.root sort4Set4.root sort4Set5.root

