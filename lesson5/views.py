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
from lesson1.models import Lesson1
from django.contrib.auth.models import User
from django.template import *
from scheduler.schedInterface import schedInterface
from scheduler.statsDisplay import statsDisplay

import charmming_config, input, output, lessonaux
import structure.models

import re
import copy
import os


def lesson5Display(request):
    if not request.user.is_authenticated():
        return render_to_response('html/loggedout.html')
    input.checkRequestData(request)
    try:
        #YP
        file = structure.models.Structure.objects.filter(owner=request.user,
                                                        selected='y')[0]
    except:
        return render_to_response('html/lesson5.html')
    #If its a lesson5 object, get the id by the file id
    if file.lesson_type == 'lesson5':
        lesson_obj = Lesson5.objects.filter(user=request.user,
                                            id=file.lesson_id)[0]
        html_step_list = lesson_obj.getHtmlStepList()
    else:
        lesson_obj = None
        html_step_list = None
    try:
        lessonproblems = LessonProblem.objects.filter(lesson_type='lesson5',
                                    lesson_id=lesson_obj.id, errorstep__lt=999)
    except:
        lessonproblems = None

    lesson_ok, dd_ok = checkPermissions(request)
    if not lesson_ok:
        return render_to_response('html/unauthorized.html')

    tmp_dict = {'lesson1':lesson_obj,
                'lessonproblems':lessonproblems,
                'html_step_list':html_step_list,
                'lesson_ok': lesson_ok, 'dd_ok': dd_ok}
    

    return render_to_response('html/lesson5.html', tmp_dict)
