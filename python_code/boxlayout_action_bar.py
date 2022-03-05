from kivy.lang import Builder
from kivy.properties import StringProperty
from kivy.uix.boxlayout import BoxLayout

Builder.load_file("boxlayout_action_bar.kv")


class BoxLayoutActionBar(BoxLayout):
    title = StringProperty()
