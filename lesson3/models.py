
# lesson 1, upload 1YJP, minimize in vacuum, solvate/neutralize, minimize again, and
# run dynamics
from django.db import models
from django.contrib.auth.models import User
from lessons.models import LessonProblem
from solvation.models import solvationTask
from minimization.models import minimizeTask
from dynamics.models import mdTask, ldTask, sgldTask
import os, re, math
import structure, lessonaux, charmming_config

class Lesson3(models.Model):
    # data for lessons (should not be overridden by subclasses)
    # specifying both user and PDBFile is redundant (since the PDBFile references the user),
    # but makes it easier to look up all lessons being done by a particular user.
    user = models.ForeignKey(User)
    nSteps = models.PositiveIntegerField(default=4)
    curStep = models.DecimalField(default=0,decimal_places=1,max_digits=3)

    
    def onFileUpload(self):
        try:
            LessonProblem.objects.get(lesson_type='lesson3',lesson_id=self.id).delete()
        except:
            pass
	try:
            oldlessons = Lesson3.objects.filter(user = self.user)
	    second_upload = 0
	    for lessn in oldlessons:
	        if lessn.curStep == 3:
		    oldlesson = lessn
                    file = structure.models.Structure.objects.get(owner=self.user,lesson_id=oldlesson.id)
	            second_upload = 1
		    break
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=file.id,errorstep=4,severity=9,description='You must perform the other steps first! This is the last step of the lesson!')
	except:
	    second_upload = 0
	if second_upload == 1:
	    try:
                file2 = structure.models.Structure.objects.get(selected='y',owner=self.user,lesson_id=self.id)
            #    filename1 = '%s/mytemplates/lessons/lesson3/1yjp-a.pdb' % charmming_config.charmming_root
            #    os.stat(filename1)
            #    #filename2 = '%s/mytemplates/lessons/lesson3/1yjp-a-goodhet.pdb' % charmming_config.charmming_root
                filename1 = file2.location + '/seq2pdb.inp'
                os.stat(filename1)
                filename2 = file2.location + '/sequ.pdb'
                os.stat(filename2)
		#filename4 = file2.location + 'new_' + file2.stripDotPDB(file2.filename) + '-a-goodhet.pdb'
	    except:
                lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=file.id,errorstep=5,severity=9,description='You did not upload 1YJP sequence properly.')
                lessonprob.save()
                return False
            #if not lessonaux.diffPDBs(file2,filename1,filename2):
            if not verifyFileByLine(filename1,"GLY ASN ASN GLN GLN ASN TYR"):
                lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=file.id,errorstep=5,severity=9,description='You did not enter the right sequence of 1YJP.')
                lessonprob.save()
                return False
	    oldlesson.curStep += 1
	    oldlesson.save()
	    file2.lesson_id = file.lesson_id
	    file2.save()
	    self.delete()
	else:
            file = structure.models.Structure.objects.filter(selected='y',owner=self.user,lesson_id=self.id)[0]
            try:
                #filename1 = '%s/mytemplates/lessons/lesson3/1yjp-sequ.pdb' % charmming_config.charmming_root
                #filename2 = file.location + 'new_' + file.stripDotPDB(file.filename) + '-sequ-pro.pdb'
                filename1 = file.location + '/seq2pdb.inp'
                os.stat(filename1)
                filename2 = file.location + '/sequ.pdb'
                os.stat(filename2)
            except:
                lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=1,severity=9,description='Please choose the option to upload your own sequence.')
                lessonprob.save()
                return False
            if not self.verifyFileByLine(filename1,"GLY ASN ASN GLN GLN ASN TYR"):
                lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=1,severity=9,description='You did not enter the right sequence of 1YJP.')
                lessonprob.save()
                return False
            self.curStep = '1'
        self.save()
        return True

    def onEditPDBInfo(self,postdata):
        return True

    def onMinimizeSubmit(self,mp,filename):
        try:
            LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=self.id)[0].delete()
        except:
            pass
        file = structure.models.Structure.objects.filter(selected='y',owner=self.user,lesson_id=self.id)[0]
        #mp = minimizeTask.objects.filter(pdb=file,selected='y')[0]
        #pdb_list = lessonaux.getPDBListFromPostdata(file,postdata)
        #acceptable_pdb_list = ['new_' + file.stripDotPDB(file.filename) + '-sequ-pro.pdb']
        #for pdb in pdb_list:
        #    if pdb not in acceptable_pdb_list:
        #        lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=2,severity=2,description='Please select all of the initial segments. Do not use a segment that has had a calculation done on it.')
        #        lessonprob.save()
        #        return False

        # check that user has the right number of steps...
        if int(mp.sdsteps) != 1000:
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=2,severity=2,description='SD steps were not set to 1000.')
            lessonprob.save()
            return False
        if int(mp.abnrsteps) != 1000:
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=2,severity=2,description='ABNR steps were not set to 1000.')
            lessonprob.save()
            return False
        if float(mp.tolg) != .01:
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=2,severity=2,description='TOLG was not set not 0.01.')
            lessonprob.save()
            return False
        if mp.usepbc == 't':
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=2,severity=2,description='PBC was used.')
            lessonprob.save()
            return False
        #2.5 Means it is running
        self.curStep = '1.5'
        self.save()
        return True

    def onMinimizeDone(self,task):
        try:
            lessonprob = LessonProblem.objects.get(lesson_type='lesson3',lesson_id=self.id)
        except:
            lessonprob = None
        #mp = minimizeTask.objects.filter(pdb=file,selected='y')[0]
        #fail = re.compile('Failed')
        if lessonprob:
            self.curStep = '1'
            self.save()
            return False
        #if fail.search(mp.statusHTML):
        if task.status=='F':
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=3,severity=9,description='The Job did not complete correctly.')
            lessonprob.save()
            self.curStep = '1'
            self.save()
            return False
        else:
            self.curStep = '2'
            self.save()
        return True

    def onSolvationSubmit(self,postdata):
        return True

    def onSolvationDone(self,file):
        return True

    def onNMASubmit(self,postdata):
        return True

    def onNMADone(self,file):
        return True

    def onMDSubmit(self,postdata,filename):
        return True

    def onMDDone(self,file):
        return True

    def onLDSubmit(self,postdata):
        return True

    def onLDDone(self,file):
        return True

    def onSGLDSubmit(self,sgldp):
        #Clear any old lessonproblems
        try:
            LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=self.id)[0].delete()
        except:
            pass
        file = structure.models.Structure.objects.filter(selected='y',owner=self.user,lesson_id=self.id)[0]
        #sgldp = sgldTask.objects.filter(pdb=file,selected='y')[0]

        #there should only be one filename in the PDBList from postdata and that should be
        #the minimized PDB

        #filename = lessonaux.getPDBListFromPostdata(file,postdata)[0]
        #if filename not in ['new_' + file.stripDotPDB(file.filename) + '-min.pdb']:
        #    lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=3,severity=2,description='Please run dynamics on the minimized PDB (-min).')
        #    lessonprob.save()
        #    return False
        if float(sgldp.fbeta) != 5.0:
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=3,severity=2,description='You used the wrong FBETA. Please use an FBETA of 5.0.')
            lessonprob.save()
            return False
        if int(sgldp.nstep) != 1000:
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=3,severity=2,description='Please set the number of steps to 1000 to continue.')
            lessonprob.save()
            return False
        self.curStep = '2.5'
        self.save()
        return True

    def onSGLDDone(self,sgldp):
        try:
            lessonprob = LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=self.id)[0]
        except:
            lessonprob = None
        #try:
        #    sgldp = sgldTask.objects.filter(pdb=file,selected='y')[0]
        #except:
        #    return False

        #fail = re.compile('Failed')
        if lessonprob:
            self.curStep = '2'
            self.save()
            return False
        #if fail.search(sgldp.statusHTML):
        if sgldp.status=='F':
            lessonprob = LessonProblem(lesson_type='lesson3',lesson_id=self.id,errorstep=3,severity=9,description='The job did not complete correctly.')
            lessonprob.save()
            self.curStep = '2'
            self.save()
            return False
        else:
            self.curStep = '3'
            self.save()
        return True

    def onRMSDSubmit(self,postdata):
        # no checking
        try:
            LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=self.id)[0].delete()
        except:
            pass
        self.curStep = '4'
        self.save()
        return True

    def onNATQSubmit(self,postdata):
        return True

    def onEnergySubmit(self,postdata):
        return True

    def onEnergyDone(self,finale):
        return True

    def onBuildStructureDone(self,file):
        return True

    def onBuildStructureSubmit(self,postdata):
        return True

    #generates html for the lesson status page
    def generateStatusHtml(self,file):
        step_status_list = []
        step_status_list.append("<tr class='status'><td class='status'>1. File Uploaded: ") 
        step_status_list.append("<tr class='status'><td class='status'>2. Minimization: ") 
        step_status_list.append("<tr class='status'><td class='status'>3. SGLD: ") 
        step_status_list.append("<tr class='status'><td class='status'>4. Compare: ") 
        #This will store all the status and the steps, clearing the template of logic
        #And only displaying the status
        try:
            lessonprob = LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=self.id)[0]
        except:
            lessonprob = None
        for i in range(self.nSteps):
            if lessonprob and lessonprob.errorstep == math.floor(self.curStep+1) and math.floor(self.curStep) == i:
                step_status_list[i] += ("<a class='failed' href='javascript:open_failure()'>Failed</a></td></tr>")
                continue
            elif (float(self.curStep)-0.5) == i and float(self.curStep) % 1 == 0.5:
                step_status_list[i] += ("<a class='running'>Running</a></td></tr>")
                continue
            elif i < float(self.curStep):
                step_status_list[i] += ("<a class='done'>Done</a></td></tr>")
                continue
            elif i >= float(self.curStep):
                step_status_list[i] += ("<a class='inactive'>N/A</a></td></tr>")
                continue
        return step_status_list 

    #Returns a list where each index corresponds to lesson progress
    #on the display lesson page
    def getHtmlStepList(self):
        #2 is running
        #1 is successfully done
        #0 is not started
        #-1 is error
        htmlcode_list = []
        for step in range(self.nSteps):
            htmlcode_list.append(0)
        if float(self.curStep) > 0:
            htmlcode_list[0] = 1
        if float(self.curStep) > 1:
            if float(self.curStep) == 1.5:
                htmlcode_list[1] = 2
            else:
                htmlcode_list[1] = 1
        if float(self.curStep) > 2:
            if float(self.curStep) == 2.5:
                htmlcode_list[2] = 2
            else:
                htmlcode_list[2] = 1
        if float(self.curStep) > 3:
            if float(self.curStep) == 3.5:
                htmlcode_list[3] = 2
            else:
                htmlcode_list[3] = 1
        try:
            lessonprob = LessonProblem.objects.filter(lesson_type='lesson3',lesson_id=self.id)[0]
            htmlcode_list[lessonprob.errorstep-1] = -1
        except:
            lessonprob = None
        return htmlcode_list

    def verifyFileByLine(self,filename,line):
        fread=open(filename,'r')
        for fline in fread:
            if fline.rstrip()==line.rstrip():
                return True
        return False
