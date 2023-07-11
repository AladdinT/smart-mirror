#!/usr/bin/python
# -*- coding: utf-8 -*-

def calc_landmark_list(image, landmarks):
    image_width, image_height = image.shape[1], image.shape[0]

    landmark_point = []

    # Keypoint
    for _, landmark in enumerate(landmarks.landmark):
        landmark_x = min(int(landmark.x * image_width), image_width - 1)
        landmark_y = min(int(landmark.y * image_height), image_height - 1)
        # landmark_z = landmark.z

        landmark_point.append([landmark_x, landmark_y])

    return landmark_point


def pre_process_landmark(landmark_list):
    temp_landmark_list = copy.deepcopy(landmark_list)

    # Convert to relative coordinates
    base_x, base_y = 0, 0
    for index, landmark_point in enumerate(temp_landmark_list):
        if index == 0:
            base_x, base_y = landmark_point[0], landmark_point[1]

        temp_landmark_list[index][0] = temp_landmark_list[index][0] - base_x
        temp_landmark_list[index][1] = temp_landmark_list[index][1] - base_y

    # Convert to a one-dimensional list
    temp_landmark_list = list(
        itertools.chain.from_iterable(temp_landmark_list))

    # Normalization
    max_value = max(list(map(abs, temp_landmark_list)))

    def normalize_(n):
        return n / max_value

    temp_landmark_list = list(map(normalize_, temp_landmark_list))

    return temp_landmark_list

########### IMPORTS ################
import socket
# from time import sleep
import csv
import copy
import itertools
import cv2 as cv
import mediapipe as mp
from model import KeyPointClassifier
from statistics import mode
from imutils.video import VideoStream
from imutils.video import FPS
import face_recognition
import imutils
import pickle
import time
import csv
import copy
import argparse
import itertools
from collections import Counter
from collections import deque
import numpy as np
import mediapipe as mp
from model import KeyPointClassifier


#Initialize 'currentname' to trigger only when a new person is identified.
currentname = "none"
#Determine faces from encodings.pickle file model created from train_model.py
myPath = "/home/pi/myQt/mySmartMirror/Plugins/myAiModule/"
encodingsP = myPath + "encodings.pickle"

# load the known faces and embeddings along with OpenCV's Haar
# cascade for face detection
print("[INFO] loading encodings + face detector...")
data = pickle.loads(open(encodingsP, "rb").read())

# initialize the video stream and allow the camera sensor to warm up
# Set the ser to the followng
# src = 0 : for the build in single web cam, could be your laptop webcam
# src = 2 : I had to set it to 2 inorder to use the USB webcam attached to my laptop
vs = VideoStream(usePiCamera=True,framerate=10).start()
#vs = VideoStream(usePiCamera=True).start()
time.sleep(2.0)
# Model load #############################################################
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    static_image_mode=True,
    max_num_hands=1,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5,
)

keypoint_classifier = KeyPointClassifier()


# Read labels ###########################################################
with open(myPath+'model/keypoint_classifier/keypoint_classifier_label.csv',
          encoding='utf-8-sig') as f:
    keypoint_classifier_labels = csv.reader(f)
    keypoint_classifier_labels = [
        row[0] for row in keypoint_classifier_labels
    ]
out_dic = { "face":"no_face" , "gesture":"no_gesture"}
HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8080         # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    # loop over frames from the video file stream
    while True:
        # grab the frame from the threaded video stream and resize it
        # to 500px (to speedup processing)
        frame = vs.read()
        frame = imutils.resize(frame,width=500)
        
        # Detect the fce boxes
        boxes = face_recognition.face_locations(frame)
        # compute the facial embeddings for each face bounding box
        encodings = face_recognition.face_encodings(frame, boxes)
        names = []

        # loop over the facial embeddings
        for encoding in encodings:
            # attempt to match each face in the input image to our known
            # encodings
            matches = face_recognition.compare_faces(data["encodings"],
                encoding)
            name = "Unknown" #if face is not recognized, then print Unknown
            
            # check to see if we have found a match
            if True in matches:
                # find the indexes of all matched faces then initialize a
                # dictionary to count the total number of times each face
                # was matched
                matchedIdxs = [i for (i, b) in enumerate(matches) if b]
                counts = {}
                
                # loop over the matched indexes and maintain a count for
                # each recognized face face
                for i in matchedIdxs:
                    name = data["names"][i]
                    counts[name] = counts.get(name, 0) + 1

                # determine the recognized face with the largest number
                # of votes (note: in the event of an unlikely tie Python
                # will select first entry in the dictionary)
                name = max(counts, key=counts.get)
                if counts[name] < 40 :
                    name = "unknown"

                #If someone in your dataset is identified, print their name on the screen
                if currentname != name:
                    currentname = name

            # update the list of names
            names.append(name)
            out_dic["face"] = name
        if not encodings:
            out_dic["face"] = "no_face"
        frame = cv.flip(frame, 1)  # Mirror display
        debug_image = copy.deepcopy(frame)

        # Detection implementation #############################################################
        frame = cv.cvtColor(frame, cv.COLOR_BGR2RGB)

        frame.flags.writeable = False
        results = hands.process(frame)
        frame.flags.writeable = True

        if results.multi_hand_landmarks is not None:
            for hand_landmarks, handedness in zip(results.multi_hand_landmarks, results.multi_handedness):
                landmark_list = calc_landmark_list(debug_image, hand_landmarks)
                # Conversion to relative coordinates / normalized coordinates
                pre_processed_landmark_list = pre_process_landmark(landmark_list)        
                # Hand sign classification
                hand_sign_id = keypoint_classifier(pre_processed_landmark_list)
                out_dic["gesture"] = keypoint_classifier_labels[hand_sign_id]
        else:
            out_dic["gesture"] = "no_gesture"
        
        print(str(out_dic))
        s.send( bytearray( str(out_dic).replace("'",'"') , 'utf-8' )  )
