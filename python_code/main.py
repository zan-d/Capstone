# GUI for capstone
from kivy.app import App
from kivy.lang import Builder
from kivy.core.window import Window
from kivy.properties import ObjectProperty, StringProperty, NumericProperty
from kivy.uix.gridlayout import GridLayout
from kivy.uix.scrollview import ScrollView
from kivy.uix.button import Button
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.clock import Clock

import pprint
import serial
import time

global plant_name
global plant_templ
global plant_temph
global plant_light
global plant_phl
global plant_phh
plant_name = "N/A"

class MainWindow(Screen):
    def __init__(self, **kwargs):
        super(MainWindow, self).__init__(**kwargs)
        if plant_name == "N/A":
            self.ids.current_plant = "No plant selected"
        else:
            self.ids.current_plant = plant_name


class DataWindow(Screen):
    d_view = ObjectProperty(None)
    name = StringProperty()

    def __init__(self, **kwargs):
        super(DataWindow, self).__init__(**kwargs)
        self.plant_data = {}
        with open('plant_data.csv') as f:
            for row in f:
                row = row.strip().split(",")
                self.plant_data[row[0]] = {}
                self.plant_data[row[0]]['temp_l'] = row[1]
                self.plant_data[row[0]]['temp_h'] = row[2]
                self.plant_data[row[0]]['light_hours'] = row[3]
                self.plant_data[row[0]]['ph_l'] = row[4]
                self.plant_data[row[0]]['ph_h'] = row[5]
            # pprint.pprint(plant_data)
        Clock.schedule_once(self.create_scrollview)

    def create_scrollview(self, dt):
        layout = GridLayout(cols=1)
        layout.bind(minimum_height=layout.setter("height"))

        for plant in self.plant_data.keys():
            button = Button(text=str(plant), size_hint=(1, 0.1), on_press=self.on_plant_select)
            layout.add_widget(button)
        scrollview = ScrollView(size=(Window.width, Window.height))
        scrollview.add_widget(layout)
        self.d_view.add_widget(scrollview)

    def on_plant_select(self, instance):
        self.name = instance.text
        print(self.name)

    def on_pre_leave(self):
        self.d_view.clear_widgets()


class WindowManager(ScreenManager):
    pass


kv = Builder.load_file("main.kv")


class MainPage(App):
    def build(self):
        return kv


if __name__ == "__main__":
    MainPage().run()
