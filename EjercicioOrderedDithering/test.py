import cv2
import numpy as np
import dither_cpp
import sys

def main():

    IMAGE_PATH = "test_image.jpg" 

    image_color = cv2.imread(IMAGE_PATH)

    if image_color is None:
        print(f"Error: No se pudo abrir la imagen '{IMAGE_PATH}'.")
        sys.exit(1)

    image_original = cv2.cvtColor(image_color, cv2.COLOR_BGR2GRAY)
    height, width = image_original.shape
    
    print(f"Procesando imagen de {width}x{height}...")

   
    flat_pixels = image_original.flatten().tolist()
    

    dithered_pixels = dither_cpp.dither_4x4(flat_pixels, width, height)


    image_dithered = np.array(dithered_pixels, dtype=np.uint8).reshape((height, width))

    
    view_original = cv2.cvtColor(image_original, cv2.COLOR_GRAY2BGR)
    view_dithered = cv2.cvtColor(image_dithered, cv2.COLOR_GRAY2BGR)


    combined = cv2.hconcat([view_original, view_dithered])

    window_name = "Comparacion: Original vs Dithering"

   
    cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
    

    cv2.imshow(window_name, combined)

    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()