#!/bin/python3

uuids={
"32a94d70-2f99-44d7-a6a0-50e8b86f6ccf":"row1" ,
"e701a55e-28bf-438c-9f93-f9a734b7ceb2":"row2" ,
"7bf7c913-cfc2-4639-9966-ea98a7d7c600":"row3" ,
"f10034be-ebbe-4f75-9678-b09c556ff283":"row4" ,
"2ccce06b-af3b-4343-81bc-b12bfaec0154":"row5" ,
"3fb4840c-f324-49e7-988f-0d47277b3582":"row6" ,
"a22e1087-ebef-4dac-b6f9-4b8a2e9bcab9":"row7" ,
"8a575308-b087-42d8-a667-f62fe33a0798":"row8" ,
"8fbd6768-2a3e-4332-aed9-726a51de05d7":"row9" ,
"b8b4f0bb-678f-49ac-b9b3-ab49af21c8a6":"row10",
"5363ba95-7dc8-4fb9-80d2-74fc8450bf7c":"row11",
"3b10cbb0-54e6-4af3-988c-4817b2404bb6":"row12",
"dd4a0694-88a2-46db-bc59-ae17141f3a01":"row13"
}

partcount = {
"U":10,
"C":25,
"L":8,
"J":4,
"H":2
}

lines = open("dotrow.kicad_sch",'r',newline='').readlines()

thisref = ""
for i,line in enumerate(lines):
    if line.startswith("\t\t(property \"Reference\" "):
        thisref = line.split("\"")[3]
    if line.startswith("\t\t\t\t(path \"/2f7734fa-ef95-48ac-9211-21614044c9f8/"):
        row = uuids[line.split('/')[2].rstrip('"\r\n')]
        ref = lines[i+1].split("\"")[1]
        if ref.startswith("#"):
            continue
        des = ref[0]
        multiplier = int(row.replace("row",""))-1
        newref = int(thisref[1:]) + multiplier * partcount[des]
        newref = des + str(newref)
        #print(row,ref,thisref,newref)
        #print(lines[i+1])
        lines[i+1] = "\t\t\t\t\t(reference \""+newref+"\")\r\n"
        #print(lines[i+1])

with open("dotrow-temp.kicad_sch",'w',newline='') as f:
    f.writelines(lines)
