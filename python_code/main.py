# GUI for capstone
from kivy.app import App
from kivy.properties import ObjectProperty, StringProperty, NumericProperty
from navigation_screen_manager import NavigationScreenManager

import pprint
import serial
import time


class MyScreenManager(NavigationScreenManager):
    pass


class iGrow(App):
    manager = ObjectProperty(None)
    current_plant = StringProperty("Please select plant")
    current_temp_l = NumericProperty()
    current_temp_h = NumericProperty()
    current_light_h = NumericProperty()
    current_ph_l = NumericProperty()
    current_ph_h = NumericProperty()

    def build(self):
        self.manager = MyScreenManager()
        return self.manager


iGrow().run()
