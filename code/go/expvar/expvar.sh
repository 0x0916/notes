# 访问 `http://localhost:4545?user=tom` 8次
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom
$ curl http://localhost:4545?user=tom
G'day tom

# 查看指标
$ curl http://localhost:4545/debug/vars | jq .
{
# 可以看到，访问了8次
  "num_calls": 8,
  "memstats": {
    "BySize": [
      {
        "Frees": 0,
        "Mallocs": 0,
        "Size": 0
      },
#  省略....
      ****
      {
        "Frees": 0,
        "Mallocs": 0,
        "Size": 17664
      }
    ],
    "DebugGC": false,
    "EnableGC": true,
    "GCCPUFraction": 0,
    "NumGC": 0,
    "PauseEnd": [
      0,
      0,
      0
    ],
    "PauseNs": [
      0,
      0,
      0
    ],
    "PauseTotalNs": 0,
    "LastGC": 0,
    "NextGC": 4194304,
    "OtherSys": 558769,
    "GCSys": 98304,
    "BuckHashSys": 2383,
    "MCacheSys": 16384,
    "MCacheInuse": 9600,
    "HeapSys": 753664,
    "HeapAlloc": 370784,
    "Frees": 120,
    "Mallocs": 5020,
    "Lookups": 25,
    "Sys": 1740800,
    "TotalAlloc": 370784,
    "Alloc": 370784,
    "HeapIdle": 32768,
    "HeapInuse": 720896,
    "HeapReleased": 0,
    "HeapObjects": 4900,
    "StackInuse": 294912,
    "StackSys": 294912,
    "MSpanInuse": 13440,
    "MSpanSys": 16384
  },
# 最后一个访问的用户
  "last_user": "tom",
  "cmdline": [
    "./expvar"
  ]
}

