from flask import Flask, request, jsonify, render_template, redirect, url_for
import pandas as pd
import pickle

# Step 1: Load the models from the 'models' folder
with open('models/kmeans_model.pkl', 'rb') as file:
    kmeans_model = pickle.load(file)

with open('models/best_dbscan_model.pkl', 'rb') as file:
    dbscan_model = pickle.load(file)

with open('models/isolation_forest_model.pkl', 'rb') as file:
    isolation_forest_model = pickle.load(file)

# Step 2: Initialize the Flask app, and set the template folder to 'pages'
app = Flask(__name__, template_folder='pages')

# Step 3: Define prediction functions

# Predict using K-Means
def predict_kmeans(dur, tot_pkts, tot_bytes, src_bytes):
    input_data = pd.DataFrame([[dur, tot_pkts, tot_bytes, src_bytes]], 
                              columns=['dur', 'tot_pkts', 'tot_bytes', 'src_bytes'])
    return kmeans_model.predict(input_data)[0]

# Predict using DBSCAN
def predict_dbscan(dur, tot_pkts, tot_bytes, src_bytes):
    input_data = pd.DataFrame([[dur, tot_pkts, tot_bytes, src_bytes]], 
                              columns=['dur', 'tot_pkts', 'tot_bytes', 'src_bytes'])
    return dbscan_model.fit_predict(input_data)[0]

# Predict anomaly using Isolation Forest
def predict_isolation_forest(dur, tot_pkts, tot_bytes, src_bytes):
    input_data = pd.DataFrame([[dur, tot_pkts, tot_bytes, src_bytes]], 
                              columns=['dur', 'tot_pkts', 'tot_bytes', 'src_bytes'])
    return isolation_forest_model.predict(input_data)[0]

# Step 4: Define the prediction route
@app.route('/predict', methods=['POST'])
def predict():
    # Get data from the form submission
    dur = float(request.form['dur'])
    tot_pkts = float(request.form['tot_pkts'])
    tot_bytes = float(request.form['tot_bytes'])
    src_bytes = float(request.form['src_bytes'])

    # Get predictions from each model
    kmeans_cluster = predict_kmeans(dur, tot_pkts, tot_bytes, src_bytes)
    dbscan_cluster = predict_dbscan(dur, tot_pkts, tot_bytes, src_bytes)
    anomaly_label = predict_isolation_forest(dur, tot_pkts, tot_bytes, src_bytes)

    # Display the predictions on a new page
    return render_template('result.html', kmeans=kmeans_cluster, dbscan=dbscan_cluster, isolation=anomaly_label)

# Serve the index.html file as the landing page
@app.route('/')
def home():
    return render_template('index.html')  # Serve the index.html page located in the 'pages' folder

# Serve the form.html file
@app.route('/form')
def form():
    return render_template('form.html')

# Step 5: Run the Flask app
if __name__ == '__main__':
    app.run(debug=False, host='127.0.0.1', port=3999)
