import re
import json
import argparse
import collections
from datetime import datetime
from datetime import time
from datetime import timedelta

#---------------------------------------------------------------------------
# class BaseItem
#---------------------------------------------------------------------------
class BaseItem(object):
    '''BaseItem'''
    def __init__(self):
        self.name = ""
        self.path = ""
        self.desc = ""
        self.start_time = ""
        self.end_time = ""
        self.elapse_time = ""
        self.items = []
    # End of __init__()
    
    #---------------------------------------------------------------------------
    # toDisplay
    #---------------------------------------------------------------------------
    def toDisplay(self):
        '''toDisplay'''
        displayMap = collections.OrderedDict()
        displayMap["Description"] = self.desc
        displayMap["StartTime"] = self.start_time
        displayMap["ElapsedTime"] = self.elapse_time
        displayMap["EndTime"] = self.end_time
        return displayMap
    # End of toDisplay()

# End of BaseItem

#---------------------------------------------------------------------------
# class build_log_timing_parser
#---------------------------------------------------------------------------
class build_log_timing_parser(object):
    '''build_log_timing_parser'''

    #---------------------------------------------------------------------------
    # __init__
    #---------------------------------------------------------------------------
    def __init__(self, log_file=None, output_file=None):
        '''__init__'''

        if log_file is None or not isinstance(log_file, basestring):
            raise ValueError("log_file must be a basestring and not None.")

        if log_file is None or not isinstance(output_file, basestring):
            raise ValueError(" output_file be a basestring and not None.")

        self._output_file = output_file
        self._timing_report = collections.OrderedDict()

        self.total_time_images = timedelta()
        self.total_time_components = timedelta()

        self.item = BaseItem()
        self.knowlageGraph = {
            "SconsTiming" : ["scons-timing:-"],
            "Start" : ["Start"],
            "Elapse" : ["Elapsed"],
            "Analysis" : ["Analysis"],
            "AnalysisTiming" : ["Analysis-Phase"],
            "Images" : ["Image"],
            "Components" : ["Component"],
            "Actions" : ["Action"],
            "ActionsTiming" : ["Action-Phase"],
            "Compiling" : ["Compiling"],
            "Archiving" : ["Archiving"],
            "Linking" : ["Linking"]
        }
        self.eventsMap = {}

        imagesList = []
        componentslist = []
        actionsList = []
        
        # for post processing 
        pp_imagesList = []
        pp_componentslist = []
        pp_actionsList = []
        
        eventList = []

        self.image_exp = 'Image (.*?) -: '
        self.component_exp = 'Component (.*?) -: '
        self.generic = 'For (.*?) -: '
        self.datetime_format = "%Y-%m-%d %H:%M:%S.%f"
        self.deltatime_format = "%H:%M:%S.%f"
        self.datetime_exp = "\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}.\d{6}"
        self.deltatime_exp = "\d{1}:\d{2}:\d{2}.\d{6}"

        with open(log_file, 'r') as fh:
            for line in fh:

                sconTiming_keys_matched = self.filter(line, self.knowlageGraph["SconsTiming"])

                analysis_keys_matched = self.filter(line, self.knowlageGraph["Analysis"])
                analysisTiming_keys_matched = self.filter(line, self.knowlageGraph["AnalysisTiming"])
                images_keys_matched = self.filter(line, self.knowlageGraph["Images"])
                components_keys_matched = self.filter(line, self.knowlageGraph["Components"])
                
                actions_keys_matched = self.filter(line, self.knowlageGraph["Actions"])
                actionsTiming_keys_matched = self.filter(line, self.knowlageGraph["ActionsTiming"])

                if sconTiming_keys_matched:
                    if analysisTiming_keys_matched:
                        analysis = self.create_item(line)
                        if analysis.elapse_time:
                            # for post processing 
                            pp_analysis = analysis.toDisplay()
                            eventList.append(analysis.toDisplay())
                            
                    elif analysis_keys_matched:
                        if images_keys_matched:
                            image = self.create_item(line)
                            if image.elapse_time:
                                image.componentslist = componentslist
                                imagesList.append(image)
                                # for post processing 
                                pp_images = image.toDisplay()
                                pp_images.update({"Components" : pp_componentslist})
                                total_time_child_comp = timedelta()
                                for o in pp_componentslist:
                                    total_time_child_comp += o["ElapsedTime"]
                                pp_images.update({"TotalTimeChild" : total_time_child_comp})
                                pp_imagesList.append(pp_images)
                                eventList.append(image.toDisplay())
                                self.total_time_images += image.elapse_time
                                # cleaning the lists
                                componentslist = []
                                pp_componentslist =[]
                                total_time_child_comp = []

                        if components_keys_matched:
                            component = self.create_item(line)
                            if component.elapse_time:
                                componentslist.append(component)
                                # for post processing
                                pp_componentslist.append(component.toDisplay())
                                eventList.append(component.toDisplay())
                                self.total_time_components += component.elapse_time

                    elif actions_keys_matched:
                        #work here
                        action_item = self.create_item(line)
                        if action_item.elapse_time:
                            action.items.append(action_item)
                            # for post processing
                            pp_actionsList.append(action_item.toDisplay())
                            eventList.append(action_item.toDisplay())

                    elif actionsTiming_keys_matched:
                        action = self.create_item(line)
                        if action.elapse_time:
                            eventList.append(action.toDisplay())
                    else:
                        generic = self.create_item(line)
                        if generic.elapse_time:
                            eventList.append(generic.toDisplay())

        total_time_child = timedelta()
        for o in pp_imagesList:
            total_time_child += o["ElapsedTime"]

        pp_analysis.update({"Images" : pp_imagesList})
        pp_analysis.update({"TotalTimeComponents" : self.total_time_components})
        pp_analysis.update({"TotaTimeChild" : total_time_child})
        pp_analysis.update({"TotalTimeImages" : self.total_time_images})

        total_time_child = timedelta()
        for o in pp_actionsList:
            total_time_child += o["ElapsedTime"]

        pp_action = action.toDisplay()
        pp_action.update({"ActionItems" : pp_actionsList})
        pp_action.update({"TotaTimeChild" : total_time_child})

        self._timing_report['Raw_Data'] = eventList
        self._timing_report['Post_Processing'] = collections.OrderedDict()
        self._timing_report['Post_Processing']['Analysis'] = pp_analysis
        self._timing_report['Post_Processing']['Action'] = pp_action
    # End of __init__() 

    #TODO:Fix this
    #---------------------------------------------------------------------------
    # __repr__
    #---------------------------------------------------------------------------
    def __repr__(self):
        '''__repr__'''
        return build_log_timing_parser()
    # End of __repr__()

    #---------------------------------------------------------------------------
    # __str__
    #---------------------------------------------------------------------------
    def __str__(self):
        '''__str__'''
        return self.timing_report
    # End of __str__()
    
    #---------------------------------------------------------------------------
    # filter
    #---------------------------------------------------------------------------    
    def filter(self, line, key_List):
        '''filter'''
        lineArr = line.split(' ')
        if len(list(set(lineArr).intersection(key_List))) > 0:
            return True
        return False
    # End of filter()

    #---------------------------------------------------------------------------
    # custom_search
    #---------------------------------------------------------------------------
    def custom_search(self, line, exp, grp):
        '''custom_search'''
        try:
            return re.search(exp, line).group(grp)
        except:
            return None
    # End of custom_search()

    #---------------------------------------------------------------------------
    # create_item
    #---------------------------------------------------------------------------
    def create_item(self, line):
        '''custom_search'''
        exp = self.generic
        if self.filter(line, self.knowlageGraph["Images"]):
            exp = self.image_exp
        elif self.filter(line, self.knowlageGraph["Components"]):
            exp = self.component_exp
        # change as per need for - compliing, Archiving, linking
        elif self.filter(line, self.knowlageGraph["Compiling"]):
            pass
        elif self.filter(line, self.knowlageGraph["Archiving"]):
            pass
        elif self.filter(line, self.knowlageGraph["Linking"]):
            pass

        return self.assign_values(line, exp)
    # End of create_item()

    #---------------------------------------------------------------------------
    # extract_time
    #---------------------------------------------------------------------------
    def extract_time(self, line, dt_exp):
        '''extract_time'''
        dt_match = self.custom_search(line, dt_exp, 0)
        if dt_match:
            if self.filter(line, self.knowlageGraph["Start"]):
                return datetime.strptime(dt_match, self.datetime_format)
            else:
                t = datetime.strptime(dt_match, self.deltatime_format)
                return timedelta(hours=t.hour, minutes=t.minute, seconds=t.second, microseconds=t.microsecond)
        return dt_match
    # End of extract_time()

    #---------------------------------------------------------------------------
    # extract_data
    #---------------------------------------------------------------------------
    def extract_data(self, line, exp, dt_exp):
        '''extract_data'''
        path = name_exp = self.custom_search(line, exp, 1)
        desc = self.custom_search(line, self.generic, 1)
        if '\\' in name_exp:
            name_exp = name_exp.split('\\')
            name_exp = name_exp[-1]
        return name_exp, path, desc, self.extract_time(line, dt_exp)
    # End of extract_data()

    #---------------------------------------------------------------------------
    # assign_values
    #---------------------------------------------------------------------------
    def assign_values(self, line, exp):
        '''assign_values'''
        start_matched = self.filter(line, self.knowlageGraph["Start"])
        elapse_matched = self.filter(line, self.knowlageGraph["Elapse"]) 

        if start_matched:
            dt_exp = self.datetime_exp
            name_exp, path, desc, ret_time = self.extract_data(line, exp, dt_exp)
            item = BaseItem()
            item.name = name_exp
            item.path = path
            item.desc = desc
            item.start_time = ret_time

        if elapse_matched:
            dt_exp = self.deltatime_exp 
            name_exp, path, desc, ret_time = self.extract_data(line, exp, dt_exp)
            if name_exp in self.eventsMap.keys():
                item = self.eventsMap[name_exp]
                item.elapse_time = ret_time
                item.end_time = item.start_time + item.elapse_time

        self.eventsMap[name_exp] = item
        return item
    # End of assign_values()

    #---------------------------------------------------------------------------
    # clear_current_components
    #---------------------------------------------------------------------------
    def clear_current_components(self):
        '''clear_current_components'''
        del self._current_component_stack[:]
    # End of clear_current_components()

    #---------------------------------------------------------------------------
    # generate_report
    #---------------------------------------------------------------------------
    def generate_report(self):
        '''generate_report'''
        with open(self._output_file, 'w') as outfile:
            outfile.write(self.timing_report)
    # End of generate_report()

    #---------------------------------------------------------------------------
    # timing_report() getter
    #---------------------------------------------------------------------------
    @property
    def timing_report(self):
        '''timing_report() getter'''
        return json.dumps(self._timing_report, sort_keys=False,
                 indent=4, cls=DatetimeEncoder)
    # End of timing_report() getter

    #---------------------------------------------------------------------------
    # timing_report() setter
    #---------------------------------------------------------------------------
    @timing_report.setter
    def timing_report(self, report):
        '''timing_report() setter'''
        self._timing_report = report
    # End of timing_report() setter

# End of BaseItem

#---------------------------------------------------------------------------
# class DatetimeEncoder
#---------------------------------------------------------------------------
class DatetimeEncoder(json.JSONEncoder):
    '''DatetimeEncoder'''
    def default(self, obj):
        try:
            return super(DatetimeEncoder, obj).default(obj)
        except TypeError:
            return str(obj)
# End of DatetimeEncoder

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="This program will generate a timing report based on the outcome of a \
    build_log.txt file. \nBoth a path to a valid build_log.txt and a destination JSON output file are required.")

    parser.add_argument("-b", "--buildlog", dest="build_log_file",
                        help="The build log file you wish to produce a timing report for.", metavar="FILE", required=True)

    parser.add_argument("-o", "--outputfile", dest="output_file",
                        help="The destination file for the JSON output.", metavar="FILE", required=True)


    args = parser.parse_args()

    myparser = build_log_timing_parser(args.build_log_file, args.output_file)

    myparser.generate_report()

    exit(0)
