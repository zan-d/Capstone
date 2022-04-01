from kivy.app import App
from kivy.clock import Clock
from kivy.lang import Builder
from kivy.properties import StringProperty, ObjectProperty
from kivy.uix.screenmanager import Screen
import serial

Builder.load_file("home.kv")


class HomeScreen(Screen):
    app = App.get_running_app()
    temp_text = StringProperty("Temperature")
    moist_text = StringProperty("Moisture")
    co2_text = StringProperty("CO2")

    def __init__(self, **kwargs):
        super(HomeScreen, self).__init__(**kwargs)
        self.fan_button = 0
        self.water_button = 0
        self.light_button = 0
        self.heat_button = 0
        Clock.schedule_interval(self.update_labels, 5)

    def plant_details(self):
        if self.app.current_plant != "Please select plant":
            # serial write ideal plant properties
            # ser.write(b"D,\n")
            print("D,{},{},{},{},{}".format(self.app.current_temp_l, self.app.current_temp_h, self.app.current_light_h,
                                            self.app.current_moist_l, self.app.current_moist_h))
            self.ids.selected_plant.text = "Ideal temperature range: {} - {} (Celcius)\n" \
                                           "Ideal soil pH range: {} - {}\n" \
                                           "Hours of light needed: {}\n" \
                                           "Moisture content range (%): {} - {}".format(self.app.current_temp_l,
                                                                                        self.app.current_temp_h,
                                                                                        self.app.current_ph_l,
                                                                                        self.app.current_ph_h,
                                                                                        self.app.current_light_h,
                                                                                        self.app.current_moist_l,
                                                                                        self.app.current_moist_h)

    def on_toggle_state(self, widget, button_name):
        if widget.state == "normal":
            state = 0
        else:
            state = 1

        if button_name == "water":
            self.water_button = state
        elif button_name == "heat":
            self.heat_button = state
        elif button_name == "light":
            self.light_button = state
        elif button_name == "fan":
            self.fan_button = state

        print("Water: {} Heat: {} Light: {} Fan: {}".format(self.water_button, self.heat_button, self.light_button,
                                                            self.fan_button))
        # serial write button states
        # ser.write(b"S,\n")

    def update_labels(self, *args):
        with serial.Serial('COM8', 9600) as ser:
            status = ser.readline()
            while ser.inWaiting() > 0:
                status = ser.readline()
            data = status
            print(data)
            data = data.strip()
            ser.close()
            # self.moist_text, self.temp_text, self.co2_text = data.decode('ascii').split(',')

