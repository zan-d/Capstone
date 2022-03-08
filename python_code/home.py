from kivy.app import App
from kivy.clock import Clock
from kivy.lang import Builder
from kivy.properties import StringProperty, ObjectProperty
from kivy.uix.screenmanager import Screen

Builder.load_file("home.kv")


class HomeScreen(Screen):
    app = App.get_running_app()

    def plant_details(self):
        if self.app.current_plant != "Please select plant":
            self.ids.selected_plant.text = "Ideal temperature range: {} - {} (Celcius)\n" \
                                           "Ideal soil pH range: {} - {}\n" \
                                           "Hours of light needed: {}".format(self.app.current_temp_l, self.app.current_temp_h, self.app.current_ph_l, self.app.current_ph_h, self.app.current_light_h)





