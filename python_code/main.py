# GUI for capstone
from kivy.app import App
from kivy.lang import Builder

import pprint
import serial
import time

from kivy.properties import ObjectProperty
from kivy.uix.widget import Widget

from navigation_screen_manager import NavigationScreenManager


class MyScreenManager(NavigationScreenManager):
    pass


class iGrow(App):
    manager = ObjectProperty(None)

    def build(self):
        self.manager = MyScreenManager()
        return self.manager


iGrow().run()
