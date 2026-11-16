# 03. Software Architecture (SWE.2)

## Operating Environment
Ubuntu 22.04, ROS 2 Humble, communication over ROS 2 topics (DDS publish-subscribe).
<!-- Add the libraries you use. -->

## Components (static aspects)
A component is a ROS 2 node. Two-digit IDs (`COMP-01`). Every SWR is allocated to at least one component.
Input / Output: IDs of the interfaces (`IF-01`) the component subscribes to / publishes.

| ID | Name | Role | Units (functions) | Input | Output | Allocated SWR |
|---|---|---|---|---|---|---|

## Interfaces
An interface is a ROS 2 topic. Two-digit IDs (`IF-01`).
Producer / Consumer: ID of the component (`COMP-01`) that publishes / subscribes to the topic.

| ID | Name (topic) | Producer | Consumer | Message Type | Content and Unit | Rate |
|---|---|---|---|---|---|---|

## Dynamic Aspects
<!-- When each component runs, cycle time, and operating modes. -->

## Diagram
```mermaid
flowchart LR
    %% Components (COMP-xx) and the interfaces (IF-xx) between them.
```

## Analysis Results
<!-- Design rationale, timing and resource analysis. -->

## Review
| Gate | Pull Request | Reviewed by | Merged on |
|---|---|---|---|
