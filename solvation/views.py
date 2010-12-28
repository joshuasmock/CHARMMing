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
from django.template.loader import get_template
from django import forms
from django.http import HttpResponseRedirect, HttpResponse
from django.shortcuts import render_to_response
from pdbinfo.models import PDBFile, PDBFileForm 
from pdbinfo.aux import checkNterPatch
from minimization.views import append_tpl
from solvation.ionization import neutralize_tpl
from solvation.models import solvationParams
from account.views import isUserTrustworthy
from pdbinfo.editscripts import generateHTMLScriptEdit
from django.contrib.auth.models import User
from django.template import *
from scheduler.schedInterface import schedInterface
from scheduler.statsDisplay import statsDisplay
import output
import re, os, copy
import lessonaux, charmming_config

#processes form data for minimization
def solvationformdisplay(request):
    if not request.user.is_authenticated():
        return render_to_response('html/loggedout.html')
    PDBFile().checkRequestData(request)
    try:
        #chooses the file based on if it is selected or not
        file =  PDBFile.objects.filter(owner=request.user,selected='y')[0]
    except:
        return HttpResponse("Please submit a structure first.")
    os.chdir(file.location)
    temp = file.stripDotPDB(file.filename)
    #creates a list of filenames associated with the PDB
    filename_list = file.getLimitedFileList('solvation')
    seg_list = file.segids.split(' ') 
    min_pdb = "new_" + file.stripDotPDB(file.filename) + "-min.pdb"
    md_pdb = "new_" + file.stripDotPDB(file.filename) + "-md.pdb"
    ld_pdb = "new_" + file.stripDotPDB(file.filename) + "-ld.pdb"
    sgld_pdb = "new_" + file.stripDotPDB(file.filename) + "-sgld.pdb"
    het_list = file.getNonGoodHetPDBList()
    tip_list = file.getGoodHetPDBList()
    seg2_list = file.getProteinSegPDBList()
    protein_list = [x for x in seg2_list if x.endswith("-pro.pdb") or x.endswith("-pro-final.pdb")]
    go_list = [x for x in seg2_list if x.endswith("-go.pdb") or x.endswith("-go-final.pdb")]
    bln_list = [x for x in seg2_list if x.endswith("-bln.pdb") or x.endswith("-bln-final.pdb")]
    nucleic_list = [x for x in seg2_list if x.endswith("-dna.pdb") or x.endswith("-dna-final.pdb") or x.endswith("-rna.pdb") or x.endswith("-rna-final.pdb")]
    userup_list = [x for x in seg2_list if not ( x.endswith("-pro.pdb") or x.endswith("-pro-final.pdb") or x.endswith("-dna.pdb") or x.endswith("-dna-final.pdb") \
                     or x.endswith("-rna.pdb") or x.endswith("-rna-final.pdb") or x.endswith("het.pdb") or x.endswith("het-final.pdb") or x.endswith("-go.pdb") \
                     or x.endswith("-go-final.pdb") or x.endswith("-bln.pdb") or x.endswith("-bln-final.pdb") ) ]
    disulfide_list = file.getPDBDisulfides()

    # Tim Miller: 03-09-2009 we need to pass a segpatch_list (containing
    # both the list of segments and their default NTER patches) to the
    # template.
    propatch_list = []
    nucpatch_list = []
    for seg in seg_list:
        if seg.endswith("-pro"):
            defpatch = checkNterPatch(file,seg)
            propatch_list.append((seg,defpatch))
        elif seg.endswith("-dna") or seg.endswith("-rna"):
            nucpatch_list.append((seg,"5TER","3TER"))

    #Adds minimized PDB for solvation
    #The below block handles the corresponding tip files to PDB segIDs
    for i in range(len(filename_list)):
        try:
            tempid = request.POST[filename_list[i]]
	    filename = request.POST[filename_list[i]]
	except:
	    try:
	        tempid = request.POST['min']
	        filename = request.POST['min']
	    except:
	        tempid = "null"
         
        if(tempid!="null"):
            try:
                if(request.POST['usepatch']):
                    file.handlePatching(request.POST)
            except:
                #If there is no patch, make sure patch_name is zero
                file.patch_name = ""
                file.save()

	    #If a user wants to solvate a structure that hasnt been minimized/or run 
	    #through charmm, then the append command must be run first
            scriptlist = []
	    if(filename != min_pdb and filename != md_pdb and filename != ld_pdb and filename != sgld_pdb):
	        seg_list = file.segids.split(' ')
		solvate_this_file = 'new_' + file.stripDotPDB(file.filename) + "-final.pdb"
		#append the file and solvate it now
	        append_tpl(request.POST, filename_list,file,scriptlist)
	        html = solvate_tpl(request,file,solvate_this_file,scriptlist)
                return HttpResponse(html)
	    else:
	        html = solvate_tpl(request,file,filename,scriptlist)
                return HttpResponse(html)
    try:
        os.stat(file.location + file.stripDotPDB(file.filename) + ".charge")
    except:
        chrge = "No charge has been calculated on the system yet."
    else:
        try:
            fp = open(file.location + file.stripDotPDB(file.filename) + ".charge", "r")
            line = fp.readline()
            fp.close()
        except:
            chrge = "No charge has been calculated on the system yet."
        else:
            chrge = "Total charge on the system (from last minimization): " + line.split('=')[1]

    n = file.natom
    timeP = "less than 5 minutes."
    if n > 5200: 
        timeP = "at least 1 hour.";
    elif n > 4000: 
        timeP = "about 30-60 minutes.";
    elif n > 2650:
        timeP = "about 20-30 minutes.";
    elif n > 1350:
        timeP = "from 5-20 minutes.";
    elif n > 800:
        timeP = "from 15 to 30 minutes.";
    neut_est = "The estimated time for neutralization is " + timeP + " (actual time will vary based on solvation parameters and system load)."

    doCustomShake = 1
    if file.ifExistsRtfPrm() < 0:
        doCustomShake = 0
    trusted = isUserTrustworthy(request.user) 
    return render_to_response('html/solvationform.html', {'filename_list': filename_list, 'propatch_list':propatch_list, 'min_pdb':min_pdb, 'md_pdb': md_pdb, \
                                 'ld_pdb': ld_pdb, 'sgld_pdb': sgld_pdb, 'het_list':het_list,'tip_list':tip_list,'protein_list':protein_list,'chrge':chrge, \
                                 'neut_est': neut_est, 'file':file, 'docustshake': doCustomShake,'trusted':trusted,'disulfide_list':disulfide_list, 'seg_list': seg_list, \
                                 'nucpatch_list': nucpatch_list, 'nucleic_list': nucleic_list, 'userup_list': userup_list, 'go_list': go_list, 'bln_list': bln_list})

def solvate_tpl(request,file,solvate_this_file,scriptlist):
    postdata = request.POST
    #deals with changing the selected minimize_params
    try:
        oldparam = solvationParams.objects.filter(pdb = file, selected = 'y')[0]
        oldparam.selected = 'n'
        oldparam.save()
    except:
        pass

    crdstring = ''
    solvate_filename = solvate_this_file
    sp = solvationParams()
    sp.selected = 'y'    
    sp.pdb = file
    sp.statusHTML = "<font color=yellow>Processing</font>"

    #file.solvation_status = "<font color=yellow>Processing</font>"
    solvation_structure = postdata['solvation_structure']

    sp.solvation_structure = solvation_structure
    sp.concentration = '0.0' # needs to be set here, will be set to proper value in ionization.py

    file.solvation_params = sp
    file.solvation_structure = solvation_structure
    #Gets the user's solvation preference, either set_pref or no_pref
    solv_pref = postdata['solv_pref']
    relative_boundary = 0
    pref_radius = 0
    if(solv_pref == 'no_pref'):
        sp.solv_pref = "no_pref"
        relative_boundary = 1
        try:
            pref_radius = postdata['no_pref_radius']
            sp.no_pref_radius = str(pref_radius)
        except:
            pref_radius = 10
            sp.no_pref_radius = '10'
    if(solv_pref == 'set_pref'):
        sp.solv_pref = "set_pref"
        pref_x = 5
        sp.pref_x = '5'
        pref_y = 5
        sp.pref_y = '5'
        pref_z = 5
        sp.pref_z = '5'
        try:
            pref_x = postdata['set_x']
            sp.pref_x = str(pref_x)
        except:
            pass

        try:
            pref_y = postdata['set_y']
            sp.pref_y = str(pref_y)
        except:
            pass

        try:
            pref_z = postdata['set_z']
            sp.pref_z = str(pref_z)
        except:
            pass

    try:
        if postdata['neutralize']:
            doneut = 1
        else:
            doneut = 0
    except:
        doneut = 0
    
    sp.save()

    # ok, let's figure out the angles
    angles = ""
    if solvation_structure == "cubic":
        angles = "90.0 90.0 90.0"
    elif solvation_structure == "rhdo":
        angles = "60.0 90.0 60.0"
    elif solvation_structure == "hexa":
        angles = "90.0 90.0 120.0"
  
    #gets pathways of rtf/prm files depending on whether the user uploaded their own
    rtf_prm_dict = file.getRtfPrmPath()
    os.chdir(file.location)
    # template dictionary passes the needed variables to the template
    template_dict = {}
    template_dict['topology_list'] = file.getTopologyList()
    template_dict['parameter_list'] = file.getParameterList()
    template_dict['filebase'] = file.stripDotPDB(file.filename)
    template_dict['solvation_structure'] = solvation_structure
    template_dict['input_file'] = file.stripDotPDB(solvate_filename)

    #If solvation structure is a sphere, it should be *2 divided by 2 because it extends from the center
    template_dict['solv_pref'] = solv_pref
    template_dict['pref_radius'] = str(float(pref_radius))
    template_dict['pref_radius_double'] = str(float(pref_radius)*2)

    if solv_pref == 'no_pref':
        if solvation_structure == "sphere":
            crdstring = "Sphere with a minimum edge distance of " + str(pref_radius)

            # silly rabbit, spheres don't have valid crystal data...
            file.crystal_x = '0'
            file.crystal_y = '0'
            file.crystal_z = '0'
        elif solvation_structure == 'choose_for_me':
            # need to prove we're using relative boundary
            file.crystal_x = '-1'
        else:
            # horrific hack alert ... we have no mechanism of actually figuring out
            # the crystal dimensions before we run CHARMM, so we're just going to save
            # the user radius value in crystal_x, y, and z ... we can use this info
            # to re-create the "real" unit cell dimension in minimization & dynamics.
            # To indicate this, we negate the value.

            # horrific hack part deux ... it's actually possible to do this, but requires
            # writing the dimension into the h eader of the PDB/CRD
            file.crystal_x = str(-float(pref_radius))
            file.crystal_y = str(-float(pref_radius))    
            file.crystal_z = str(-float(pref_radius))
        # end  if(solv_pref == 'no_pref')
    else:
        # here we want to check the user values on things -- fortunately, this makes our lives easy
        template_dict['pref_x'] = pref_x
        template_dict['pref_z'] = pref_z
        if solvation_structure == "sphere":
            file.crystal_x = str(pref_x)
            file.crystal_y = '0.0'
            file.crystal_z = '0.0'
            crdstring = "Sphere with a radius of " + str(file.crystal_x)
        elif solvation_structure == 'choose_for_me':
            return HttpResponse("Sorry, the auto-chooser does not work with custom dimensions!")
        elif solvation_structure == "hexa":
            file.crystal_x = str(pref_x)
            file.crystal_y = '0.0'
            file.crystal_z = str(pref_z)
            template_dict['tmgreaterval'] = max(file.crystal_x, file.crystal_z)
            template_dict['tmsecondval'] = min(file.crystal_x, file.crystal_z)
            crdstring = "Hexagonal crystal structure with x, z dimensions of " + str(file.crystal_x) + " and " + str(file.crystal_z)
        else:
            # else clause handles cubic and rhdo
            file.crystal_x = str(pref_x)
            file.crystal_y = '0.0'
            file.crystal_z = '0.0'
            crdstring = solvation_structure + " crystal structure with a dimension of " + str(file.crystal_x)
    
    #set angles for run
    template_dict['angles'] = angles   


    # now we write out the newly solvated structure and we are done...
    #Check and see if user supplies their own topology/paramter file
    template_dict['file_location'] = file.location
    template_dict['RtfPrm'] = ''
    if file.ifExistsRtfPrm() < 0:
        template_dict['RtfPrm'] = 'y'
    template_dict['shake'] = request.POST.has_key('apply_shake')
    if request.POST.has_key('apply_shake'):
        template_dict['which_shake'] = postdata['which_shake']
        template_dict['qmmmsel'] = ''
        if postdata['which_shake'] == 'define_shake':
            template_dict['shake_line'] = postdata['shake_line']
            if postdata['shake_line'] != '':
                file.checkForMaliciousCode(postdata['shake_line'],postdata)
     
    template_dict['output_name'] = "new_" + file.stripDotPDB(file.filename) + "-solv" 						
    template_dict['headqmatom'] = "* solvation: @headstr"

    t = get_template('%s/mytemplates/input_scripts/solvation_template.inp' % charmming_config.charmming_root)
    charmm_inp = output.tidyInp(t.render(Context(template_dict)))
    
    user_id = file.owner.id
    solvate_input_filename = file.location + "charmm-" + file.stripDotPDB(file.filename) + "-solv.inp"
    inp_out = open(solvate_input_filename,'w')
    inp_out.write(charmm_inp)
    inp_out.close()  
    #change the status of the file regarding solvation
    scriptlist.append(solvate_input_filename)
    file.save()
    if doneut:
#        neutralize(file,postdata,scriptlist)
        neutralize_tpl(file,postdata,scriptlist)
    elif postdata.has_key('edit_script') and isUserTrustworthy(request.user):
        return generateHTMLScriptEdit(charmm_inp,scriptlist,'solvation')
    else:
        file.save() 
        si = schedInterface()
        newJobID = si.submitJob(user_id,file.location,scriptlist)
        if file.lesson_type:
            lessonaux.doLessonAct(file,"onSolvationSubmit",postdata)
        file.solvation_jobID = newJobID
        sstring = si.checkStatus(newJobID)
        file.solvation_params.statusHTML = statsDisplay(sstring,newJobID)
	file.solvation_params.save()
        file.save() 
    return "Done"


