#!/bin/bash
# Opens 4 terminals: simulator, mission_manager, evaluation, autonomous_driving.
#
# usage: ./launch.sh [map]
#   map: easy (default), normal, or hard (resources/csv/<map>/)
#
# Missions: set each switch to true or false below. Positions and parameters of the
# missions are fixed per map; only these switches can be changed.
ICE=true            # icy region            Mission.road_condition = "Ice"
SLOPE=true          # uphill and downhill   Mission.road_slope = "Up" / "Down"
SCHOOL_ZONE=true    # school zone, 30 kph   Mission.speed_limit, shown 50 m before the zone
SCC=true            # vehicles ahead        Mission.objects
MERGE=true          # lane closure, merge   Mission.objects (open lane: left or right at random)
PARKING=true        # park at the track end Mission.parking
LANE_LOSS=true      # lane markings missing in some sections (lane_points)

MAP=${1:-easy}
if [ ! -d "resources/csv/$MAP" ]; then
    echo "unknown map '$MAP' (use easy, normal, or hard; run from the repository root)" >&2
    exit 1
fi
echo "map: $MAP  ice: $ICE  slope: $SLOPE  school_zone: $SCHOOL_ZONE  scc: $SCC  merge: $MERGE  parking: $PARKING  lane_loss: $LANE_LOSS"

terminator -e  " source ~/.bashrc && source install/setup.bash && ros2 launch simulator simulation.launch.xml map:=$MAP lane_loss:=$LANE_LOSS " &
sleep 3
terminator -e  " source ~/.bashrc && source install/setup.bash && ros2 launch mission_manager mission_manager.launch.xml map:=$MAP ice:=$ICE slope:=$SLOPE school_zone:=$SCHOOL_ZONE scc:=$SCC merge:=$MERGE parking:=$PARKING " &
sleep 3
terminator -e  " source ~/.bashrc && source install/setup.bash && ros2 launch evaluation evaluation.launch.xml map:=$MAP parking:=$PARKING " &
sleep 3
terminator -e  " source ~/.bashrc && source install/setup.bash && ros2 launch autonomous_driving autonomous_driving.launch.xml "
