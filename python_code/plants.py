import pprint
from kivy.app import App
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.lang import Builder
from kivy.properties import ObjectProperty, StringProperty
from kivy.uix.button import Button
from kivy.uix.gridlayout import GridLayout
from kivy.uix.screenmanager import Screen
from kivy.uix.scrollview import ScrollView

Builder.load_file("plants.kv")


class PlantsWindow(Screen):
    plants_view = ObjectProperty(None)
    name = StringProperty()

    def __init__(self, **kwargs):
        super(PlantsWindow, self).__init__(**kwargs)
        with open('plant_data.csv') as f:
            for row in f:
                row = row.strip().split(",")
                self.plants = Plants(row[0], row[1], row[2], row[3], row[4], row[5])
        # pprint.pprint(Plants.PlantData)
        Clock.schedule_once(self.create_scrollview)

    def create_scrollview(self, dt):
        layout = GridLayout(cols=1)
        layout.bind(minimum_height=layout.setter("height"))

        for plant in Plants.PlantData:
            button = Button(text=str(plant.name), size_hint=(1, 0.1), on_press=self.on_plant_select, background_normal='', color=(0, 0, 0))
            layout.add_widget(button)
        scrollview = ScrollView(size=(Window.width, Window.height))
        scrollview.add_widget(layout)
        self.plants_view.add_widget(scrollview)

    def on_plant_select(self, instance):
        self.name = instance.text
        # print(self.name)
        App.get_running_app().current_plant = self.name
        for plant in Plants.PlantData:
            if plant.name == self.name:
                App.get_running_app().current_temp_l = plant.temp_l
                App.get_running_app().current_temp_h = plant.temp_h
                App.get_running_app().current_light_h = plant.light_h
                App.get_running_app().current_ph_l = plant.ph_l
                App.get_running_app().current_ph_h = plant.ph_h
                break
        App.get_running_app().manager.get_screen('HomeScreen').plant_details()
        App.get_running_app().manager.pop()


class Plants:
    PlantData = []

    name = ""
    temp_l = 0
    temp_h = 0
    light_h = 0
    ph_l = 0.0
    ph_h = 0.0

    def __init__(self, name, temp_l, temp_h, light_h, ph_l, ph_h):
        if not(any(x.name == name for x in self.PlantData)):
            self.PlantData.append(self)
            self.name = name
            self.temp_l = temp_l
            self.temp_h = temp_h
            self.light_h = light_h
            self.ph_l = ph_l
            self.ph_h = ph_h

