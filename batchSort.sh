parallel --xapply ./sort4 ::: 80 121 161 201 241 ::: 120 160 200 240 285 ::: sortSet1 sortSet2 sortSet3 sortSet4 sortSet5
cd root-files/
hadd -f "$1" sortSet1.root sortSet2.root sortSet3.root sortSet4.root sortSet5.root

