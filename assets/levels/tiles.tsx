<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.1" name="tiles" tilewidth="32" tileheight="32" tilecount="50" columns="10">
 <image source="../sheet.png" trans="000000" width="320" height="128"/>
 <tile id="9">
  <animation>
   <frame tileid="9" duration="300"/>
   <frame tileid="19" duration="300"/>
  </animation>
 </tile>
 <wangsets>
  <wangset name="road" type="edge" tile="-1">
   <wangcolor name="road" color="#ff0000" tile="-1" probability="1"/>
   <wangtile tileid="5" wangid="0,0,1,0,1,0,0,0"/>
   <wangtile tileid="6" wangid="0,0,1,0,1,0,1,0"/>
   <wangtile tileid="7" wangid="0,0,0,0,1,0,1,0"/>
   <wangtile tileid="8" wangid="0,0,0,0,1,0,0,0"/>
   <wangtile tileid="15" wangid="1,0,1,0,1,0,0,0"/>
   <wangtile tileid="16" wangid="1,0,1,0,1,0,1,0"/>
   <wangtile tileid="17" wangid="1,0,0,0,1,0,1,0"/>
   <wangtile tileid="18" wangid="1,0,0,0,1,0,0,0"/>
   <wangtile tileid="25" wangid="1,0,1,0,0,0,0,0"/>
   <wangtile tileid="26" wangid="1,0,1,0,0,0,1,0"/>
   <wangtile tileid="27" wangid="1,0,0,0,0,0,1,0"/>
   <wangtile tileid="28" wangid="1,0,0,0,0,0,0,0"/>
   <wangtile tileid="35" wangid="0,0,1,0,0,0,0,0"/>
   <wangtile tileid="36" wangid="0,0,1,0,0,0,1,0"/>
   <wangtile tileid="37" wangid="0,0,0,0,0,0,1,0"/>
  </wangset>
 </wangsets>
</tileset>
