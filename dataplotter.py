from matplotlib import pyplot as plt


print("Stats dict:")
statsstr = ''
while True:
    line = input()
    statsstr += line
    if statsstr.count('{') == statsstr.count('}'):
        break
stats = eval(statsstr)
diffic = list(stats.keys())
diffic.sort()
plt.xticks(diffic)
plt.plot(diffic, [stats[i]['Max'] for i in diffic], marker='o', mew=2)
plt.plot(diffic, [stats[i]['Avg'] for i in diffic], marker='o', mew=2)
plt.show()

