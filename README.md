# Automatic Classification of Ictal Medical Reports

This project implements different information retrieval and natural
language processing techniques in order to classify medical EEG
reports in two classes: records in which the patient experienced a
seizure and the records in which the patient did not experience a
seizure.

## Files and Directories 
- [data](https://github.com/autopear/CIS5538-Project/tree/master/data/isip/data/tuh_eeg/v0.6.0/edf): contains the EEG medical reports directory tree. The data is a subset of the TUH EEG database, and it consists of .txt files.
- [lists](https://github.com/autopear/CIS5538-Project/tree/master/lists):
    - [`dilantin.list`][]: patients who take the anti-epileptic drug Dilantin
    - [`epileptogenic.list`][]: reports that contain the word epileptogenic
    - [`onset.list`][]: reports that contain the word onset
    - [`reports_seizures.list`][]: list of reports where a seizure occured 
      (currently incomplete)
    - [`seiz_none.list`][]: reports that contain the string 'Seizures: None'
    - [`seizures.list`][]: reports that contain the string 'Seizures: '
    - [`seiz_none_diff.list`][]: diff between seiz_none.list and seizures.list
    - [`total.list`][]: list of all the .txt files in the data directory
