# GUI for capstone
from kivy.app import App
from kivy.lang import Builder
from kivy.uix.screenmanager import ScreenManager, Screen


class MainWindow(Screen):
    pass


class DataWindow(Screen):
    pass


class WindowManager(ScreenManager):
    pass


kv = Builder.load_file("capstone.kv")


class MainPage(App):
    def build(self):
        return kv


if __name__ == "__main__":
    MainPage().run()
