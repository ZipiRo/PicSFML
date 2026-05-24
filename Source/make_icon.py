import argparse
from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument("--input", required=True)
parser.add_argument("--output", required=True)

args = parser.parse_args()

img = Image.open(args.input)

img.save(
    args.output,
    format="ICO",
    sizes=[(16,16), (32,32), (48,48), (64,64), (256,256)]
)