#include "route_model.h"
#include <iostream>

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
	int counter = 0;
  	for( Model::Node node : this->Nodes())
    {
      m_Nodes.push_back(Node(counter, this, node));
      counter++;
    }
  	CreateNodeToRoadHashmap();
}

void RouteModel::CreateNodeToRoadHashmap() {
	for(const Model::Road &road : this-> Roads())
    {
      	if (road.type == Model::Road::Type::Footway) {
        	continue;
        }
      
        for (int node_idx : Ways()[road.way].nodes) {
          if (node_to_road.find(node_idx) == node_to_road.end()) {
            node_to_road[node_idx] = std::vector<const Model::Road *> ();
          }
          node_to_road[node_idx].push_back(&road);
        }
    }
}

RouteModel::Node * RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
  	Node *closest_node = nullptr;
	for(int index : node_indices) {
    	auto node = this->parent_model->SNodes()[index];
      	if(node.visited || this->distance(node) == 0){
        	continue;
        }
      
        if (closest_node == nullptr
            || this->distance(node) < this->distance(*closest_node)) {
          closest_node = &parent_model->SNodes()[index];
        }
        
    }
  	return closest_node;
}

void RouteModel::Node::FindNeighbors() {
  	for (auto & road : parent_model->node_to_road[this->index]){
    	RouteModel::Node *neighbor_pointer = this->FindNeighbor(parent_model->Ways()[road->way].nodes);
      	if (neighbor_pointer) {
        	this->neighbors.emplace_back(neighbor_pointer);
        }
    }
}

RouteModel::Node &RouteModel::FindClosestNode(float x, float y){
	Node input;
  	input.x = x;
  	input.y = y;
  	
  	float min_dist = std::numeric_limits<float>::max();
  	float dist;
  	int closest_idx;
  
  	for (const Model::Road &road : Roads()) {
    	if (road.type == Model::Road::Type::Footway) {
        	continue;
        }
      
        for (int node_idx : Ways()[road.way].nodes) {
          dist = input.distance(SNodes()[node_idx]);
          if (dist < min_dist) {
            closest_idx = node_idx;
            min_dist = dist;
          }
        }
    }
  	return SNodes()[closest_idx];
}