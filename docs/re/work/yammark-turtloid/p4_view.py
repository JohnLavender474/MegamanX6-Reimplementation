import sys
from PIL import Image
d = "ora/data/"
names = {"main": "layer2.png", "deco": "layer3.png", "bg": "layer4.png"}
import xml.etree.ElementTree as ET
for e in ET.parse("ora/stack.xml").iter("layer"):
    print(e.get("name"), e.get("src"))
