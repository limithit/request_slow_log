# request_slow_log    

---  

I don't know who the original author is, so I can't post LICENSE. I only saw this module occasionally and had implemented the same functionality before, 

so I didn't repeat the wheel building, but simply modified some of the code logic to make it easy to read.

---

# build install  

```

# cd nginx-xxxx  
# ./configure --add-module=/path/request_slow_log/ngx_http_log_request_speed
```  

##  log_request_speed_filter  
 Enable switch parameter `on` or `off`
``` 
 Syntax:  log_request_speed_filter on/off;
 Default: —
 Context: http server location 
  
```       
##  log_request_speed_filter_timeout
Parameters in `seconds` s or `milliseconds` ms
```
 Syntax:  log_request_speed_filter_timeout 1s/100ms;
 Default: 1s
 Context: http server location 
```    
