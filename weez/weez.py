import cv2
import numpy as np
import argparse
import shutil

def parse_args():
    """ Argument parser for configuration file.
    """
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--img',
                        help='Image filepath',
                        required=False,
                        type=str)
    parser.add_argument('opts', default=None, nargs=argparse.REMAINDER)
    args = parser.parse_args()

    return args

def print_image(img_path : str, rows : int, cols : int, resize_mode : str = "fit") -> str:
    rows = rows * 2

    print(f"Rows: {rows}, Cols: {cols}")

    cv2_img = cv2.imread(img_path)
    orig_w, orig_h = cv2_img.shape[:2]
    
    if resize_mode == 'fit':
        if orig_w < orig_h:
            scale = cols / orig_w
        else:
            scale = rows / orig_h

        w = int(orig_w * scale)
        h = int(orig_h * scale)

    print(f"Width: {w}, Height: {h}")

    img = np.array(cv2.resize(cv2_img, (w, h)))
    
    rows, cols = img.shape[:2]

    #weezer = 'weezer'
    ratio = 2

    for i in range(0, rows, ratio):
        for j in range(0, cols, ratio):
            k_matrix = img[i:(i+ratio), j:(j+ratio)]
            k = np.average(np.average(k_matrix, axis=0), axis=0)
            #if i == 2 and j >= 24 and j <= 29:
            #    print(f"\x1b[48;2;{k[2]};{k[1]};{k[0]};2;30m{weezer[j-24]} \x1b[0m", end='')
            #else:
            # print(f"Matrix: \n{k_matrix}")
            # print(f"Average: \n{k}")
            # exit()
            print(f"\x1b[48;2;{int(k[2])};{int(k[1])};{int(k[0])}m  \x1b[0m", end='')
        print()

if __name__ == '__main__':
    args = parse_args()
    if args.img:
        img_path = args.img
    else:
        img_path = '/home/fsmith/weezer.png'

    cols, lines = shutil.get_terminal_size()

    print_image(img_path, lines, cols, resize_mode = "fit")

# python3 weez.py --img /path/to/img
