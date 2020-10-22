#! /bin/bash
for varible1 in {16000..16100};do ping -c 1 -W 1 vcm-$varible1.vm.duke.edu > /dev/null;if [ $? -eq 0 ]; then echo "vcm-$varible1.vm.duke.edu up";else echo "vcm-$varible1.vm.duke.edu down";fi; done

