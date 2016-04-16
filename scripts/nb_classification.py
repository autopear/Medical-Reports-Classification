#!/usr/bin/env python

# file: $CIS5538-Project/scripts/feats.py
#
# revision history:
#  20160411 (SL): initial version
#

#------------------------------------------------------------------------------

# import sklearn modules
#
from __future__ import print_function

from sklearn.datasets import load_files
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.feature_extraction.text import HashingVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn import metrics
from sklearn.naive_bayes import MultinomialNB
from sklearn import cross_validation


# import other modules
#
import logging
from optparse import OptionParser
import sys
from time import time
import numpy as np


# Display progress logs on stdout
#
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(message)s')

# parse commandline arguments
op = OptionParser()
op.add_option("--lsa",
              dest="n_components", type="int",
              help="Preprocess documents with latent semantic analysis.")
op.add_option("--no-minibatch",
              action="store_false", dest="minibatch", default=True,
              help="Use ordinary k-means algorithm (in batch mode).")
op.add_option("--no-idf",
              action="store_false", dest="use_idf", default=True,
              help="Disable Inverse Document Frequency feature weighting.")
op.add_option("--use-hashing",
              action="store_true", default=False,
              help="Use a hashing feature vectorizer")
op.add_option("--n-features", type=int, default=10000,
              help="Maximum number of features (dimensions)"
                   " to extract from text.")
op.add_option("--verbose",
              action="store_true", dest="verbose", default=False,
              help="Print progress reports inside k-means algorithm.")

#print(__doc__)
#op.print_help()

(opts, args) = op.parse_args()
#if len(args) > 0:
#    op.error("this script takes no arguments.")
#    sys.exit(1)

#------------------------------------------------------------------------------
#
# Load Training Data
#
#------------------------------------------------------------------------------

# define data directory
#
# The samples are assumed to be stored at two level folder structure
#  the highest level is given by the directories named after the categories
#  the lower level contains the files (names of files are irrelevant).
#
print(">> loading training data into memory...");

container_path_train = \
    "/home/tud22978/_projects/CIS5538-Project/test_data/dataset_SP/train";
dataset_train = load_files(container_path_train);

print("   %d documents" % len(dataset_train.data));
print("   %d categories" % len(dataset_train.target_names));
print("   classes 1: %s" % dataset_train.target_names[0]);
print("   classes 2: %s" % dataset_train.target_names[1]);

labels = dataset_train.target;



#X_train, X_test, y_train, y_test = cross_validation.train_test_split(
#    dataset_train.data, dataset_train.target, test_size=0.5, random_state=0)

print()
#------------------------------------------------------------------------------
#
# Training Feature Extraction
#
#------------------------------------------------------------------------------

# Extract the TF-IDF features from the training dataset
#
print(">> Extracting training TF-IDF features...");
t0 = time()
vectorizer = TfidfVectorizer(max_df=0.5, max_features=917,
                             min_df=2, stop_words='english',
                             use_idf=True, encoding='latin-1')
X = vectorizer.fit_transform(dataset_train.data)

print("   done in %fs" % (time() - t0));
print("   n_samples: %d, n_features: %d" % X.shape)

#------------------------------------------------------------------------------
#
# Train Bayes Classifier
#
#------------------------------------------------------------------------------

print(">> training classifier...");

classifier = MultinomialNB(alpha=1.0, class_prior=[0.75,0.25]).fit( \
    X, dataset_train.target)

#------------------------------------------------------------------------------
#
# Load Testing Data
#
#------------------------------------------------------------------------------

# define data directory
#
# The samples are assumed to be stored at two level folder structure
#  the highest level is given by the directories named after the categories
#  the lower level contains the files (names of files are irrelevant).
#
print(">> loading testing data into memory...");

container_path_test = \
    "/home/tud22978/_projects/CIS5538-Project/test_data/dataset_SP/test";
dataset_test = load_files(container_path_test);

print("   %d documents" % len(dataset_test.data));

print()

#------------------------------------------------------------------------------
#
# Testing Feature Extraction
#
#------------------------------------------------------------------------------

# Extract the TF-IDF features from the training dataset
#
print(">> Extracting testing TF-IDF features...");
t0 = time()
vectorizer = TfidfVectorizer(max_df=0.5, max_features=917,
                             min_df=2, stop_words='english',
                             use_idf=True, encoding='latin-1')
X_t = vectorizer.fit_transform(dataset_test.data)

print("   done in %fs" % (time() - t0));
print("   n_samples: %d, n_features: %d" % X_t.shape)

#------------------------------------------------------------------------------
#
# Evaluation
#
#------------------------------------------------------------------------------

predicted = classifier.predict(X_t)

#for doc, category in zip(dataset_test.filenames, predicted):
#    print('%r => %s' % (doc, dataset_train.target_names[category]))

score = classifier.score(X_t, dataset_test.target)

print()
print("   DETECTION RATE: %f" % score)
print()

#------------------------------------------------------------------------------
#
# Writing evaluation to file
#
#------------------------------------------------------------------------------
i=0.05;
for count in range(0, 19):
    j = 1 - i
    classifier = MultinomialNB(alpha=1.0, class_prior=[i,j]).fit( \
    X, dataset_train.target)
    predicted = classifier.predict(X_t)
    score = classifier.score(X_t, dataset_test.target)
    print("  nrml: %f | seiz: %f |  score:  %f" % (i, j, score))
    i = i + 0.05
    count = count + 1
