from kivy.app import App
from kivy.lang import Builder
from kivy.properties import StringProperty, ObjectProperty
from kivy.uix.screenmanager import Screen

Builder.load_file("home.kv")


class HomeScreen(Screen):
    name = StringProperty()




