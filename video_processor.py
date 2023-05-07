import cv2
import skimage
import os

resolution = (32, 32)
playback_resolution = (720, 720)
fps = 60

def play_visualized_video(input):
    capture = cv2.VideoCapture(input)

    # Loop through the video frames
    while True:
        ret, frame = capture.read()
        if ret == True:
            resized_frame = cv2.resize(frame, resolution, fx=0, fy=0, interpolation = cv2.INTER_AREA)
            processed_frame = skimage.util.img_as_ubyte(resized_frame)

            upscaled_processed_frame = cv2.resize(processed_frame, playback_resolution, fx=0, fy=0, interpolation = cv2.INTER_NEAREST)

            cv2.imshow("frame", upscaled_processed_frame)
            key = cv2.waitKey(int((1 / fps) * 1000)) 
            if key == ord('q'):
                break
        else:
            break
        
    capture.release()
    cv2.destroyAllWindows()

# Play all the files in the videos directory
for file in os.listdir("videos"):
    play_visualized_video("videos/" + file)