#!/usr/bin/python

#This script is used to extract failed test cases from emdrv XMLs
#and generate HTML for attaching with email

from subprocess import call
import os
import xml.etree.ElementTree as ET
import traceback

#Global

#Report HTML
reportFile="./scripts/emdrvreportfile.html"

def procXmlLog():
  global reportFile
  searchStr = ("error")
  for filename in os.listdir('./test/out'):
    try:
      tree = ET.parse('./test/out/' + filename)
    except:
      print "Error parsing " + filename 
      traceback.print_exc()
      print
      os.sys.exit(1)
      
    nodes = {}

    for testcase in tree.findall("testcase"):
      for val in testcase.getchildren():
        if testcase.find(searchStr) is not None:
          nodes.update({testcase.attrib['name']:val.attrib['message']})

    if nodes:
      f = open(reportFile,"a")
      for key,value in nodes.iteritems():
        st = "<tr>"
        st += "<td align=\"center\">" + os.path.splitext(filename)[0] + "</td>\n"
        st += "<td>" + key + "</td><td>" + value + "</td>"
        st += "</tr>"
        f.write(st)
      f.close()

if __name__ == "__main__":
  if os.path.exists(reportFile):
    os.remove(reportFile)
  f = open(reportFile,'w')
  st = "<html><body><table border=\"1\" cellpadding=\"1\" cellspacing=\"1\" style=\"width: 100%\">  <tbody>\n"
  st += "<tr bgcolor=\"#99CCFF\" style=\"font-weight:bold\" align=\"center\"><td>Family</td><td>Failed Test Case</td><td>Error</td></tr>\n"
  f.write(st)
  f.close()

  procXmlLog()

  f = open(reportFile,'a')
  st = "</tbody></table></body></html>"
  f.write(st)
  f.close()
