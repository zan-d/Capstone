from kivy.app import App
from kivy.clock import Clock
from kivy.lang import Builder
from kivy.properties import StringProperty, ObjectProperty
from kivy.uix.screenmanager import Screen
import serial
from time import sleep

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

        self.com = 'COM8'
        self.baudrate = 57600
        self.ser = serial.Serial(self.com, self.baudrate)
        sleep(2)
        Clock.schedule_interval(self.update_labels, 10)
        Clock.schedule_interval(self.send_emtpyLine, 2)

    def plant_details(self):
        if self.app.current_plant != "Please select plant":
            # serial write ideal plant properties
            # with serial.Serial(self.com, self.baudrate) as ser:
            status = "D,{},{},{},{},{}\n".format(self.app.current_temp_l, self.app.current_temp_h,
                                                 self.app.current_light_h,
                                                 self.app.current_moist_l, self.app.current_moist_h)
            self.ser.write(status.encode('ascii'))

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

        # print("Water: {} Heat: {} Light: {} Fan: {}".format(self.water_button, self.heat_button, self.light_button,
        #                                                     self.fan_button))
        print("S,{},{},{},{}".format(self.water_button, self.heat_button, self.light_button, self.fan_button))

        # serial write button states
        # ser = serial.Serial(self.com, self.baudrate)
        # sleep(2)
        status = "S,{},{},{},{};".format(self.water_button, self.heat_button, self.light_button, self.fan_button)
        self.ser.write(status.encode('ascii'))

    def send_emptyLine(self, *args):




    def update_labels(self, *args):
        # ser = serial.Serial(self.com, self.baudrate)
        # sleep(2)
        # get the most recent data
        # while self.ser.inWaiting() > 0:
        #     status = self.ser.readline()
        #     status = status.decode('ascii').strip()
        #     if "echo" in status:
        #         print(status)

        status = self.ser.readline()
        status = status.decode('ascii')

        data = status
        print(data)
        if data.count(",") == 2:
            moist, temp, co2 = data.split(',')
            self.moist_text = "Moisture: " + moist
            self.temp_text = "Temperature: " + temp
            self.co2_text = "CO2: " + co2
