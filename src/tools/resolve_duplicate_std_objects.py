#!/usr/bin/python
#
# The purpose of this script is to find obejcts that normally belong
# to the Standard Objects library but were copied to the user-defined
# library thus creating duplicate IDs. Script finds such objects in
# the specified data file and prints sed commands that change the IDs
# to make them unique. The script does not modify the data file in any
# way.
#
# If a copy of a duplicate object in the user-defined library is
# identical to the object in the Standard library, it is removed. If
# the copy has been modified, the id is changed to be unique and all
# references to it are corrected. Obejcts are compared as text lines
# regardless of the contents.
#
# This script does not use fwbuilder DTD and operates on xml files as
# plain text, it can be used with files of old or different versions
# regardless of fwbuilder DTD.

import getopt
import time
import re
import sys

NEW_ID_PREFIX = '%s' % int(time.time())
generated_commands = []

def usage():
    print '%s data_file.fwb objects_init.xml' % sys.argv[0]

def command_to_delete(id):
    generated_commands.append("""sed '/id="%s"/d'""" % id)

def command_to_change(id):
    new_id = '%s_%s' % (id, NEW_ID_PREFIX)
    generated_commands.append("""sed 's/id="%s"/id="%s"/'""" % (id, new_id))
    generated_commands.append("""sed 's/ref="%s"/ref="%s"/g'""" % (id, new_id))

def main():
    try:
        data_file = sys.argv[1]
        std_file = sys.argv[2]
    except IndexError,e:
        usage()
        sys.exit(1)

    std_objects = {}

    objects_of_interest = ['TCPService', 'UDPService', 'ICMPService', 'IPService',
                           'Network', 'Host', 'IPv4', 'IPv6',
                           'ObjectGroup', 'ServiceGroup', 'IntervalGroup',
                           'Library']
    objects_we_never_delete = ['ObjectGroup', 'ServiceGroup', 'IntervalGroup', 'Library']

    object_type_re = re.compile('<([^ ]+) ')
    object_id_re = re.compile('id="([^\"]+)"')
    object_ref_re = re.compile('ref="([^\"]+)"')
    object_comment_re = re.compile('comment="([^\"]*)"')

    # first, read standard objects and catalog all lines that have "id=" in them
    for line in open(std_file):
        line = line.strip()
        m = object_id_re.search(line)
        if m:
            id = m.group(1)
            std_objects[id] = line

    # now scan the data file, find all object definitions and compare
    for line in open(data_file):
        line = line.strip()
        m = object_type_re.match(line)
        if m:
            obj_type = m.group(1)
            if obj_type in objects_of_interest:
                m = object_id_re.search(line)
                if m:
                    id = m.group(1)
                    if id in std_objects:
                        print 32 * '-'
                        print 'Find duplicate object:'
                        print 'Object in the data file:'
                        print line
                        print 'Object in the standard objects file:'
                        print std_objects[id]
                        # One of the typical cases is when only comment has changed
                        file_line_no_comment = None
                        std_line_no_comment = None
                        m1 = object_comment_re.match(line)
                        m2 = object_comment_re.match(std_objects[id])
                        if m1:
                            file_comment = m1.group(1)
                            file_line_no_comment = line.replace('comment="%s"' % file_comment, '')
                        if m2:
                            std_comment = m2.group(1)
                            std_line_no_comment = line.replace('comment="%s"' % std_comment, '')

                        if line == std_objects[id]:
                            if obj_type not in objects_we_never_delete:
                                print 'Objects are IDENTICAL'
                                command_to_delete(id)
                            else:
                                print 'Objects are IDENTICAL but we cant delete'
                                command_to_change(id)

                        elif (file_line_no_comment and
                              std_line_no_comment and
                              file_line_no_comment == std_line_no_comment):
                            print 'Only comments are DIFFERENT'
                            print file_line_no_comment
                            print std_line_no_comment
                            command_to_change(id)

                        else:
                            print 'Objects are DIFFERENT'
                            command_to_change(id)

    print 'cat %s | %s' % (data_file, ' | '.join(generated_commands))

if __name__ == '__main__':
    main()
