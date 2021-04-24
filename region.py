import cv2
import numpy as np



class regionOfInterest(object, ):

    def roi(self, img, vertices, channel_count = 3):

        self.img = img
        self.vertices = vertices
        self.channel_count = channel_count

        self.mask = np.zeros_like(self.img)



        self.mask_color = (255, ) * self.channel_count

        cv2.fillPoly(self.mask, self.vertices, self.mask_color)

        self.masked_image = cv2.bitwise_and(self.img, self.mask)

        return self.masked_image



