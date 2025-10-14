#!/usr/bin/env python
# coding: utf-8

# In[2]:


import json
import itertools

officeTypes = {"Other":0,"SEZ":1,"Government":2,"IT":3,"Construction":4,"Medical":5}

DAYS_TILL_NO_ATTENDANCE_RESTRICTION = 56-8+4 # 8 to account for shoft to 8 to match bangalore cases. + 4 to account for lockdown extension till April 24.
FIRST_PERIOD_ATTENDANCE = 14-4 #4 added to earlier period
SECOND_PERIOD_ATTENDANCE = 28
THIRD_PERIOD_ATTENDANCE = 42

TOTAL_NUM_DAYS = 150


optionslist = [[0], [0,1],[0,0.33],[0,0.5],[0],[0]]
lastperiodoptions = [1]

options = list(itertools.product(*optionslist))
count = 0
for x in lastperiodoptions:
    for (a,b,c,d,e,f) in options:
        #str_o = str(optionslist[0].index(a))+str(optionslist[1].index(b))+str(optionslist[2].index(c))+str(optionslist[3].index(d))+str(optionslist[4].index(e))+str(optionslist[5].index(f))
        str_o = "_count_" + str(count)
        #str_o +="_SEZ_"+str(b)
        #str_o +="_Gov_"+str(c)
        #str_o +="_IT_"+str(d)
        #str_o +="_Cons_"+str(e)
        #str_o +="_Med_"+str(f)
        #print(count, a,b,c,d,e,f)
        count+=1
        #str_o = str_o + "_lastperiod_" + str(x)
        interventions = []
        for i in range(1,TOTAL_NUM_DAYS+1):
            w = {"id":i}
            if i <= DAYS_TILL_NO_ATTENDANCE_RESTRICTION: #until May 3
                w[officeTypes['Other']] = 1
                w[officeTypes['SEZ']] = 1
                w[officeTypes['Government']] = 1
                w[officeTypes['IT']] = 1
                w[officeTypes['Construction']] = 1
                w[officeTypes['Medical']] = 1
            elif i <= DAYS_TILL_NO_ATTENDANCE_RESTRICTION + FIRST_PERIOD_ATTENDANCE:#until May 31
                w[officeTypes['Other']] = a
                w[officeTypes['SEZ']] = b
                w[officeTypes['Government']] = c
                w[officeTypes['IT']] = d
                w[officeTypes['Construction']] = e
                w[officeTypes['Medical']] = f
            elif i <= DAYS_TILL_NO_ATTENDANCE_RESTRICTION + FIRST_PERIOD_ATTENDANCE + SECOND_PERIOD_ATTENDANCE: #until July 12
                w[officeTypes['SEZ']] = 1
                w[officeTypes['Government']] = 1
                w[officeTypes['IT']] = 0.5
                w[officeTypes['Construction']] = 1
                w[officeTypes['Medical']] = 1
                w[officeTypes['Other']] = 0.5
            else:
                w[officeTypes['SEZ']] = x
                w[officeTypes['Government']] = x
                w[officeTypes['IT']] = x
                w[officeTypes['Construction']] = x
                w[officeTypes['Medical']] = 1
                w[officeTypes['Other']] = x
            interventions.append(w)

        f = open("attendance" + str_o + ".json", "w+")
        f.write(json.dumps(interventions))
        f.close


# In[ ]:




