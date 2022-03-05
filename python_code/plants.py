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
        self.plants_view.add_widget(scrollview)

    def on_plant_select(self, instance):
        self.name = instance.text
        print(self.name)

    def on_pre_leave(self):
        self.plants_view.clear_widgets()
