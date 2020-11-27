CREATE TABLE road_damage (
  gc_pub_sub_id VARCHAR(16) PRIMARY KEY,
  device_id VARCHAR(24),
  event VARCHAR(50),
  damage_event VARCHAR(255),
  lat VARCHAR(255),
  lon VARCHAR(255),
  published_at DATETIME
 );
