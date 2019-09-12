parallel --xapply ./sort3_v3 ::: 80 121 161 201 231 261 ::: 120 160 200 230 260 285 ::: sort3Set1 sort3Set2 sort3Set3 sort3Set4 sort3Set5 sort3Set6
cd root-files/
hadd -f "$1" sort3Set1.root sort3Set2.root sort3Set3.root sort3Set4.root sort3Set5.root sort3Set6.root

