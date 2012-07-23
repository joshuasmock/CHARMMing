#
#                            PUBLIC DOMAIN NOTICE
#
#  This software/database is a "United States Government Work" under the
#  terms of the United States Copyright Act.  It was written as part of
#  the authors' official duties as a United States Government employee and
#  thus cannot be copyrighted.  This software is freely available
#  to the public for use.  There is no restriction on its use or
#  reproduction.
#
#  Although all reasonable efforts have been taken to ensure the accuracy
#  and reliability of the software and data, NIH and the U.S.
#  Government do not and cannot warrant the performance or results that
#  may be obtained by using this software or data. NIH, NHLBI, and the U.S.
#  Government disclaim all warranties, express or implied, including
#  warranties of performance, merchantability or fitness for any
#  particular purpose.
from django import forms
from django.http import HttpResponseRedirect, HttpResponse
from django.shortcuts import render_to_response
from account.views import isUserTrustworthy
from account.views import checkPermissions
from structure.models import Structure
from lessons.models import LessonProblem
from lesson2.models import Lesson2
from lesson3.models import Lesson3
from django.contrib.auth.models import User
from django.template import *
from scheduler.schedInterface import schedInterface
from scheduler.statsDisplay import statsDisplay
import input
import re
import copy
import os

#Displays lesson1 page
def lesson3Display(request):
    if not request.user.is_authenticated():
        return render_to_response('html/loggedout.html')
    input.checkRequestData(request)
    #try:
    file = Structure.objects.filter(owner=request.user,selected='y')[0]
    #except:
    #    return render_to_response('html/lesson3.html')
    #If its a lesson1 object, get the id by the file id
    if file.lesson_type == 'lesson3':
        lesson_obj = Lesson3.objects.filter(user=request.user,id=file.lesson_id)[0] 
        html_step_list = lesson_obj.getHtmlStepList()
    else:
        lesson_obj = None
        html_step_list = None
    try:
        lessonprob_obj = LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=lesson_obj.id)[0]
    except:
        lessonprob_obj = None
    
    lesson_ok, dd_ok = checkPermissions(request) 
    if not lesson_ok:
        return render_to_response('html/unauthorized.html')
    return render_to_response('html/lesson3.html',{'lesson3':lesson_obj,'lessonproblem':lessonprob_obj,'html_step_list':html_step_list, 'lesson_ok': lesson_ok, 'dd_ok': dd_ok})
   

