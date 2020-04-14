import numpy as np
import cv2

original = cv2.imread("lena_ascii.pgm")
decodificado = cv2.imread("lena_ascii.huff.pgm")

def calculate_psnr(img1, img2, max_value=255):
    mse = np.mean((np.array(img1, dtype=np.float32) - np.array(img2, dtype=np.float32)) ** 2)
    if mse == 0:
        return 100
    return 20 * np.log10(max_value / (np.sqrt(mse)))

d = calculate_psnr(original,decodificado)

print "O PSNR eh: "
if d == 100:
    print "+infinito"
else:
    print d