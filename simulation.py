#!/usr/bin/env python3

#
#   Note this script is not designed to be run from this directory this is here so that it can be shared with the rest
#   of the lab group as an example. This should be run from outside of the ns3 folder, currently in an example folder
#
#  /
#      ns-3-allinone/
#          .....
#      experiment/
#          simulation.py
#
#   This script should be run from the experiment directory
#

import sem
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import os
import requests
import time
from datetime import timedelta


num_runs = 500
experimentName = 'speedfalloff'

ns_path = '../allinone2/ns-3.32'
script = 'speedfalloff-plots'
discord_url = os.environ.get('DISCORD_URL')


param_combination = {
    'runTime': [4 * 60 * 60],
    'ticTime': [1],
    'totalNodes': [1],
    'areaWidth': 1000,
    'areaLength': 1000,
    'mobilityModel': ['waypoint'],
    'minSpeed': [0],
    'maxSpeed': [30],
    'minPauseTime': [0],
    'maxPauseTime': [0],
    'changeTime': [0],
    'changeDistance': [0],
    'walkMode': ['time'],
}

##############################
# Start setting up functions
##############################


@sem.utils.yields_multiple_results
@sem.utils.output_labels(['Time', 'nodeID', 'posX', 'posY', 'speedX', 'speedY', 'speed'])
@sem.utils.only_load_some_files(['stdout'])
def get_all(result):
    data = []
    for line in result['output']['stdout'].strip().split('\n'):
        row = [float(i) for i in line.split(' ')]
        row += [ np.sqrt(np.power(row[-1], 2) + np.power(row[-2], 2)) ]
        data += [ row ]
    return data

def sendNotification(message):

    #os.system(f'notify-send "NS3 sims" "{message}"')

    if discord_url is not None:
        requests.post(discord_url, json={"content": message})

    print(message)

def createPlot():

    data = campaign.get_results_as_dataframe(get_all, params=param_combination)
    p = sns.lineplot(data=data, x='Time', y='speed')
    p.set_title(f'Average speed of {num_runs} nodes over 4 hours of simulated time in a\n1km x 1km area using the Random Waypoint Mobility Model')
    p.set_ylabel("Node speed (m/s)")
    p.set_xlabel("Time (s)")

    avgSpeed = np.mean(data['speed'])

    plt.axhline(y=avgSpeed, color='red', label=str(f'average velocity {avgSpeed} m/s'))
    plt.legend()


    plt.savefig(os.path.join(figure_dir, f'speed decay.pdf'))
    plt.clf()
    plt.close()


    nodeId=10
    singleNode = data.query(f'RngRun == {nodeId} and nodeID == 0')
    #sns.lineplot(data=singleNode, x='posX', y='posY', sort=False, lw=1)
    plt.plot(singleNode['posX'], singleNode['posY'])
    plt.title(f'Trajectory of node {nodeId} over a 4 hour simulation in a 1km x 1km area\n using the Random Waypoint Mobility Model')
    plt.xlabel("x location (m)")
    plt.ylabel("y location (m)")
    plt.plot(singleNode.iloc[0]['posX'], singleNode.iloc[0]['posY'], 'om', label='Start')
    plt.plot(singleNode.iloc[-1]['posX'], singleNode.iloc[-1]['posY'], 'vk', label='End')
    plt.legend()

    plt.savefig(os.path.join(figure_dir, f'node {nodeId} trajectory.pdf'))
    plt.clf()
    plt.close()




def getTimes(condition=lambda r: True):
    res = campaign.db.get_complete_results()
    times = [ r['meta']['elapsed_time'] for r in res if condition(r)]
    return {
        'total': timedelta(seconds=sum(times)),
        'min': timedelta(seconds=min(times)),
        'max': timedelta(seconds=max(times)),
        'avg': timedelta(seconds=np.mean(times))
    }

def getRuntimeInfo():
    times = getTimes()

    str = f"Running all Simulations took: avg={times['avg']}s\ttotal={times['total']}\tmin={times['min']}\tmax={times['max']}"

    times = getTimes(lambda r: r['meta']['exitcode'] == 0)
    return f"{str}\nRunning success Simulations took: avg={times['avg']}s\ttotal={times['total']}\tmin={times['min']}\tmax={times['max']}\n"


def runSimulation(campaign):
    totalSims = len(sem.manager.list_param_combinations(param_combination)) * num_runs
    toRun = len(campaign.get_missing_simulations(sem.manager.list_param_combinations(param_combination), runs=num_runs))
    sendNotification(f'Starting simulations, {toRun} of {totalSims} simulations to run')

    campaign.run_missing_simulations(param_combination, runs=num_runs, stop_on_errors=False)

    sendNotification("Simulations have finished running")

    ## print some of the information about the run
    sendNotification(getRuntimeInfo())




##############################
#     Run the simulations
##############################

if __name__ == "__main__":
    campaign_dir = os.path.join(os.getcwd(), experimentName)
    figure_dir = os.path.join(os.getcwd(), f'{experimentName}_figures')

    if not os.path.exists(figure_dir):
        os.makedirs(figure_dir)


    campaign = sem.CampaignManager.new(ns_path, script, campaign_dir, check_repo=False, max_parallel_processes=14)

    runSimulation(campaign)
    start = time.time()
    createPlot()
    end = time.time()
    print(f'Figures generated in: {timedelta(seconds=end - start)}')
