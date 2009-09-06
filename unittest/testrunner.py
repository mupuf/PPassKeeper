#!/usr/bin/env python

import os
import sys
import glob
import subprocess
import ppasskeeper

test_directory = os.path.dirname(sys.argv[0])

class TestException(Exception):
    pass

class CppRunner:
    cpp_directory = os.path.join(test_directory, 'cpp')
    cpp_files = glob.glob(os.path.join(test_directory, 'cpp/*.cpp'))
    cpp_names = map(lambda x: os.path.basename(x)[:-4], cpp_files)
    clean = False
    def __init__(self, name):
        self.name = name
        if not name in self.cpp_names:
            raise TestException('No such C++ test: %s' % name)
        if not CppRunner.clean:
            self.make('clean')
            CppRunner.clean = True

    def make(self, rule=None):
        if rule == None:
            rule = self.name
        if subprocess.call(['make', '--silent', '-C', self.cpp_directory, rule]) != 0:
            raise TestException('make failed with rule "%s"' % rule)

    def run(self):
        print 'Running C++ test: %s' % self.name
        try:
            self.make()
        except TestException:
            print '> BUILD FAILED!'
            return
        p = subprocess.Popen([os.path.join(self.cpp_directory, self.name), module_id], stdout=subprocess.PIPE)
        output = p.communicate()[0]
        results = eval(output)
        print_results(results)

def print_results(results):
    if results['failure']:
        print '> FAILURE!'
    elif results['failed'] > 0:
        print '> RUN OK, WITH ERRORS'
    else:
        print 'GOOD!'
    print '%d assertions passed, %d failed' % (results['passed'], results['failed'])

def main():
    module_list = ppasskeeper.ModuleList()
    global module_id
    try:
        module_id = sys.argv[1]
    except IndexError:
        module_id = None
    if module_id is None or not module_id in module_list:
        print 'Specify a valid module as first argument:'
        for module in module_list:
            print module
        sys.exit(1)

    # Run all the C++ tests
    for name in CppRunner.cpp_names:
        CppRunner(name).run()

if __name__ == '__main__':
    sys.exit(main())
