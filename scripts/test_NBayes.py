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
from sklearn.decomposition import TruncatedSVD
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.feature_extraction.text import HashingVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import Normalizer
from sklearn import metrics
from sklearn.cluster import KMeans, MiniBatchKMeans
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import MultinomialNB

# import other modules
#
import logging
from optparse import OptionParser
import sys
from time import time
import numpy as np
import pandas as pd

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
# Load Data
#
#------------------------------------------------------------------------------

# define data directory
#
# The samples are assumed to be stored at two level folder structure
#  the highest level is given by the directories named after the categories
#  the lower level contains the files (names of files are irrelevant).
#
print(">> loading data into memory...");
container_path = "/home/tud22978/_projects/CIS5538-Project/test_data";
dataset = load_files(container_path);

print("   %d documents" % len(dataset.data));
print("   %d categories" % len(dataset.target_names));

labels = dataset.target;

# Split data into training and testing sets
#
docs_train, docs_test, y_train, y_test = train_test_split(
    dataset.data, dataset.target, test_size=0.5);

print()
#------------------------------------------------------------------------------
#
# Feature Extraction
#
#------------------------------------------------------------------------------

# Extract the TF-IDF features from the dataset
#
print(">> Extracting TF-IDF features...");
t0 = time()
vectorizer = TfidfVectorizer(max_df=0.5, max_features=3,
                             min_df=2, stop_words='english',
                             use_idf=True, encoding='latin-1')
X = vectorizer.fit_transform(dataset.data)

print("   done in %fs" % (time() - t0));
print("   n_samples: %d, n_features: %d" % X.shape)

# Reduce the dimensionality of the features with LSA
#
#print(">> Reducing Dimensionality with LSA...")
#t0 = time()
#svd = TruncatedSVD(1000)
#normalizer = Normalizer(copy=False)
#lsa = make_pipeline(svd, normalizer)

#X = lsa.fit_transform(X)

#[n_samples, n_features] = X.shape

#print("   done in %fs" % (time() - t0))

#explained_variance = svd.explained_variance_ratio_.sum()
#print("   Explained variance of the SVD step: {}%".format(
#        int(explained_variance * 100)))

#print();

#------------------------------------------------------------------------------
#
# Train Bayes Classifier
#
#------------------------------------------------------------------------------

classifier = MultinomialNB().fit(X, dataset.target)

docs_new = ['patient experimenced a seizure', 'there were no seizure events']
vectorizer = TfidfVectorizer(max_df=1, max_features=10000,
                        min_df=1, stop_words='english',
                        use_idf=True, encoding='latin-1')

X_new = vectorizer.fit_transform(docs_new)

print("   n_samples: %d, n_features: %d" % X_new.shape)

predicted = classifier.predict(X_new)

for doc, category in zip(docs_new, predicted):
    print('%r => %s' % (doc, dataset.target_names[category]))
